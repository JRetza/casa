//# FlagVersion.cc: Manage flag versions
//# Copyright (C) 1994,1995,1996,1997,1998,1999,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id$



//# Includes

#include <cmath>
#include <casa/Exceptions/Error.h>
#include <casa/iostream.h>
#include <casa/fstream.h>
#include <casa/stdio.h>

#include <casa/Exceptions.h>

#include <tables/Tables/TableParse.h>
#include <tables/Tables/TableGram.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/TableLock.h>
#include <tables/Tables/TableIter.h>
#include <tables/Tables/ScaColDesc.h>
#include <tables/Tables/ArrColDesc.h>
#include <tables/Tables/ScalarColumn.h>
#include <tables/Tables/ArrayColumn.h>
#include <tables/Tables/TableDesc.h>
#include <tables/Tables/TableInfo.h>
#include <tables/Tables/SetupNewTab.h>
#include <tables/Tables/Table.h>
#include <tables/Tables/TableRecord.h>
#include <tables/Tables/TiledShapeStMan.h>

#include <casa/Arrays/ArrayMath.h>
#include <casa/Arrays/MatrixMath.h>
#include <casa/Arrays/ArrayError.h>
#include <casa/Arrays/Matrix.h>

#include <casa/Utilities/DataType.h>
#include <casa/Quanta/MVTime.h>
#include <casa/System/ProgressMeter.h>

#include <casa/OS/RegularFile.h>
#include <casa/OS/Directory.h>
#include <casa/OS/SymLink.h>

#include <tableplot/TablePlot/FlagVersion.h>

namespace casa { //# NAMESPACE CASA - BEGIN

#define TMR(a) "[User: " << a.user() << "] [System: " << a.system() << "] [Real: " << a.real() << "]"
#define MIN(a,b) ((a)<(b) ? (a) : (b))
#define MAX(a,b) ((a)>(b) ? (a) : (b))

#define LOG0 0

String FlagVersion::clname = "FlagVersion";
/* Constructor */
FlagVersion::FlagVersion(String intab, String dataflagcolname, 
       String rowflagcolname)
{

   String fnname= "constructor";
   log = SLog::slog();
   tab_p = Table(intab, Table::Update);
   
   /* Check if intab is the root table */
   if( ! tab_p.isRootTable() ) 
      log->out(String("Table: ") + tab_p.tableName() 
         + " is not the root Table. It is a sub-selection. " 
         + "Flag versions will be created, but will not be " 
         + "interchangeable across selections. ",
         fnname, clname, LogMessage::WARN);
   
   /* Record the names of columns to be used for flags */
   dataflagcolname_p = dataflagcolname;
   rowflagcolname_p = rowflagcolname;
   
   fcol_p = tab_p.tableDesc().isColumn(dataflagcolname_p);
   frcol_p = tab_p.tableDesc().isColumn(rowflagcolname_p);

   if(!fcol_p && !frcol_p) 
   {
      log->out(String("Table: ") + tab_p.tableName() 
         + " has no flag columns. Cannot save flag versions",
         fnname, clname, LogMessage::WARN); 
      FlagVersionError(
    String("No Flag Columns exist for this table. Cannot save flag versions"));
   }
   
   tabname_p = tab_p.tableName();

   // Check if tabname_p is a symbolic link. if so - expand !!
   //cout << "before : " << tabname_p <<endl;
   File rootfile;
   Directory fileroot(tabname_p);
   if (fileroot.exists()) {
      if (fileroot.isSymLink()) {
         rootfile = SymLink(fileroot).followSymLink();
         tabname_p = rootfile.path().expandedName();
       }
   }
   //cout << "after : " << tabname_p <<endl;
        
   nrows_p = tab_p.nrow();

   /* Make this a directory of flag tables */
   String flagdir(".flagversions");
   verlistfile_p = tabname_p + flagdir + String("/FLAG_VERSION_LIST");
   flagtablename_p = tabname_p + flagdir + String("/flags.");

   //cout << "Working with " << tabname_p + flagdir << endl;

   /* Make sure the directory exists */
   Directory dir( tabname_p + flagdir );
   if( ! dir.exists() ) dir.create();
   else
   {
      if ( !dir.isWritable() || !dir.isReadable() )
         log->out("Flag Table directory is not accesible",
             fnname, clname, LogMessage::WARN);
   }
   if( !dir.exists() )
      FlagVersionError(String("Flag directory does not exist."));
   
   /* Make sure the version list file exists */
   RegularFile listfile(verlistfile_p);
   if( !listfile.exists() ) listfile.create();
   else
   {
      if ( !listfile.isWritable() || !listfile.isReadable() )
         log->out("Flag Version list file is not accessible",
              fnname, clname, LogMessage::WARN); 
   }
   if( !listfile.exists() )
      FlagVersionError(String("flag version file does not exist "));
   
   
   /* Read in the list of versions, and check that the 
      corresponding flag tables also exist */
   readVersionList();
   
}

/*********************************************************************************/

/* Destructor */
FlagVersion::~FlagVersion()
{
#if LOG0
    log->FnPass("~FlagVersion", clname);
#endif
}

/*********************************************************************************/

/*********************************************************************************/
Bool FlagVersion::doesVersionExist( String versionname )
{
   /* check if this versionname exists. */
   Bool exists = False;
   for(Int i=0;i<(Int)versionlist_p.nelements();i++)
      if(versionlist_p[i].matches(versionname)) exists = True;

   return exists;
}
/*********************************************************************************/

/* Return a vector of strings, one for each line in the version-list file */
Vector<String> FlagVersion::getVersionList()
{
   return commentlist_p;
}

/*********************************************************************************/
Bool FlagVersion::attachFlagColumns(String version, 
                      ScalarColumn<Bool> &rowflag, 
                      ArrayColumn<Bool> &flag, Table &subtab)
{
        //os.FnEnter( "attachFlagColumns", 
        //       "attachFlagColumns( version, rowflag, flag, subtab )" );

   /* Find this version. Set to currentflagtable */
   Bool exists = doesVersionExist(version);

   Table currentflagtable;//, subflagtable;
   
   if(exists && !version.matches("main"))
   {
      String tabvername = flagtablename_p + version;
      currentflagtable = Table(tabvername, Table::Update);
   }
   else /* If not found, or if main is asked for, then set to main... tab_p */
   {
      currentflagtable = tab_p;
   }

   subflagtable_p = currentflagtable(subtab.rowNumbers());
   
   if(frcol_p) 
       rowflag.attach(subflagtable_p,rowflagcolname_p);
   if(fcol_p) 
       flag.attach(subflagtable_p,dataflagcolname_p);
   
   return True;
}

/*********************************************************************************/
Bool FlagVersion::saveFlagsInto( Table &fromFTab, Table &toFTab, String merge )
{
    String fnname= "saveFlagsInto";
#if LOG0 
    log->FnEnter(fnname + "(fromFTAb, toFTab, merge)", clname);
#endif 

#if LOG0 
    log->out(String("From table: ")+ fromFTab.tableName() +
             "To table:   "+ toFTab.tableName() +
             "Merge type: "+ merge,
             fnname, clname, LogMessage::DEBUGGING);
#endif

        if( ! merge.matches("replace") &&
            ! merge.matches("and") &&
            ! merge.matches("or") ) merge = String("replace");
        
   if(frcol_p)
   {
      ScalarColumn<Bool> fromRowFlag(fromFTab, rowflagcolname_p);
      ScalarColumn<Bool> toRowFlag(toFTab, rowflagcolname_p);
      
      if( merge.matches("and") )
      {
         Vector<Bool> rfc = fromRowFlag.getColumn();
         rfc *= toRowFlag.getColumn();
         toRowFlag.putColumn(rfc);
      }
      if( merge.matches("or") )
      {
         Vector<Bool> rfc = fromRowFlag.getColumn();
         rfc += toRowFlag.getColumn();
         toRowFlag.putColumn(rfc);
      }
      if( merge.matches("replace") ) {
         toRowFlag.putColumn(fromRowFlag.getColumn());
      }
   }
   
   if(fcol_p)
   {
      ArrayColumn<Bool> fromFlag(fromFTab, dataflagcolname_p);
      ArrayColumn<Bool> toFlag(toFTab, dataflagcolname_p);

      Bool fixedshape=False;

      /* Check if the FLAG column has the same shape for all rows */
      /* If so - then loop through in large chunks.
         If not - read/write row by row. 
         TODO : This is slow, and TableIterator is perhaps a faster way
                 to do this.

         getColumn/putColumn would work for small MSs but runs out of
         memory for large MSs.
      */
           
      // Check if fixed shape. probably the better way... but it sometimes
      // tells us that fixed shaped columns are not!
      TableDesc td = fromFTab.actualTableDesc();
      fixedshape = (td.columnDesc( dataflagcolname_p )).isFixedShape();

      if (! fixedshape) {
	/* If we can read the column with getColumn, it is fixed shape.
	   If we couldn't, it may not be fixed shape or we ran out of memory.
	*/
	try  {
	  Array<Bool> rfs = fromFlag.getColumn();
	  fixedshape = True;
	}
	catch(...) {
	}
      }
      
      //cout << " is Fixed Shape : " << fixedshape << endl;
      
      if( fixedshape ) { 
	
	IPosition shape(fromFlag.shape(0));
	/* Process large chunks at a time in order to avoid overhead, e.g. 100 MB.
	   Empirically, processing row by row has just a small overhead (3 times)
	   so we just need to process many rows at a time.
	*/		
	uInt chunk_rows = 100*1024*1024 / (shape(0) * shape(1)) + 1;
	Array<Bool> arr1;
	Array<Bool> arr2;
	for(unsigned i=0;i<nrows_p;i += chunk_rows)
	  {
	    unsigned j = i + chunk_rows - 1;
	    if (!(j < nrows_p)) j = nrows_p - 1;
	    RefRows arraySection(i, j);

	    fromFlag.getColumnCells(arraySection,arr1,True);

	    if( merge.matches("and") ) {
	      toFlag.getColumnCells(arraySection,arr2,True);
	      arr2 *= arr1;
	    }
	    else if (merge.matches("or")) {
	      toFlag.getColumnCells(arraySection,arr2,True);
	      arr2 += arr1;
	    }
	    else if (merge.matches("replace")) {
	      arr2.assign(arr1);
	    } 
	    toFlag.putColumnCells(arraySection,arr2);
	  }
      }
      else {
	// Check if DATA_DESC_ID is a column
	//    if so, use the TableIterator on this.
	// For now, go row by row....
	Array<Bool> arr1;
	Array<Bool> arr2;
	ProgressMeter pm(0, nrows_p, "Saving flags");
	for(unsigned i=0;i<nrows_p;i++)
	  {
	    if (i % 16384 == 0) {
	      pm.update(i);
	    }
	    fromFlag.get(i,arr1,True);
	    if( merge.matches("and") ) {
	      toFlag.get(i,arr2,True);
	      arr2 *= arr1;
	    }
	    else if (merge.matches("or")) {
	      toFlag.get(i,arr2,True);
	      arr2 += arr1;
	    }
	    else if (merge.matches("replace")) {
	      arr2 = arr1;
	    }
	    toFlag.put(i,arr2);
	  }
      }
   } // end if fcol_p
   
   toFTab.flush();

   return True;
}
/*********************************************************************************/

/* If this flag version exists, save current flags into this version */
/* If it doesnt exist, create a new flag version and save current flags there */
Bool FlagVersion::saveFlagVersion( String versionname , 
             String comment , String merge)
{
   String fnname= "saveFlagVersion";
   Bool exists = doesVersionExist(versionname);
   String tabvername = flagtablename_p + versionname;

   /* If doesn't exist, say so and make a new version. */
   /* Attach all current variables to this table */
   if(!exists)
   {
      log->out(String("Creating new backup flag file called ") +
             versionname, fnname, clname, LogMessage::NORMAL); 
   
      ofstream listfile;
      listfile.open(verlistfile_p.data(),ofstream::app);
      listfile << versionname << " : " << comment << endl; 
      listfile.close();

      /* Create a new Table with the standard Flag Table descriptor */
      TableDesc td("", versionname, TableDesc::Scratch);
      td.comment() = "TablePlot Flag Table : " + versionname;
      
      if(fcol_p) {
          td.addColumn (ArrayColumnDesc<Bool> (dataflagcolname_p, 2));
          td.defineHypercolumn("TiledFlag", 3,
                               stringToVector(dataflagcolname_p));
      }
      if(frcol_p) td.addColumn (ScalarColumnDesc<Bool> (rowflagcolname_p));

      SetupNewTable aNewTab(tabvername, td, Table::New);

      // FLAG hyperColumn
      {
          ArrayColumn<Bool> fromFlag(tab_p, dataflagcolname_p);
          
          uInt tile_size = 1024*1024; // bytes
          IPosition tileShape(3, fromFlag.shape(0)(0), fromFlag.shape(0)(1), 
			      tile_size*8 / (fromFlag.shape(0)(0) * fromFlag.shape(0)(1)));
      
          TiledShapeStMan flagStMan("TiledFlag", tileShape);
          aNewTab.bindColumn(dataflagcolname_p, flagStMan);
      }

      Table ftab(aNewTab, Table::Plain, nrows_p);

      saveFlagsInto( tab_p, ftab, String("replace") );

      readVersionList();
   }
   
   /* Save current main flags into this version. */
   if(exists && !versionname.matches("main")) 
   {
      Table ftab(tabvername, Table::Update);
      if( ftab.nrow() != tab_p.nrow() ) 
          log->out(String("nrows don't match !! "),
                fnname, clname, LogMessage::NORMAL);

      saveFlagsInto( tab_p, ftab, merge );
      
      ofstream listfile;
      listfile.open(verlistfile_p.data());
      for(Int i=0;i<(Int)versionlist_p.nelements();i++)
      {
         if( ! versionlist_p[i].matches("main") )
         {
            if(versionlist_p[i].matches(versionname) && comment.length()>0)
               commentlist_p[i] = versionname + String(" : ") + comment;
            listfile << commentlist_p[i] << endl; 
         }
      }
      listfile.close();
   }
   
   return True;
}

/*********************************************************************************/
Bool FlagVersion::restoreFlagVersion( String versionname, String merge )
{
   String fnname= "restoreFlagVersion";
   Bool exists = doesVersionExist(versionname);
   String tabvername = flagtablename_p + versionname;
   
   /* If doesn't exist, say so and return. */
   if(!exists)
   {
      log->out(String("Flag version ") +  versionname + 
          " does not exist", fnname, clname, LogMessage::WARN);
      return False;
   }
   
   /* Save current flags from this version to the main table. */
   if(exists && !versionname.matches("main")) 
   {
      Table ftab(tabvername, Table::Update);
      saveFlagsInto( ftab, tab_p, merge );
   }
   
   return True;
}

/*********************************************************************************/
Bool FlagVersion::deleteFlagVersion( String versionname )
{
   String fnname= "deleteFlagVersion";
   /* Check if this version exists */
   /* check if this versionname exists. */
   Bool exists = False;
   for(Int i=0;i<(Int)versionlist_p.nelements();i++)
      if(versionlist_p[i].matches(versionname)) exists = True;

   if(!exists)
   {
      log->out(String("Flag version ") +  versionname + 
          " does not exist", fnname, clname, LogMessage::WARN);
      return False;
   }

   if(versionname.matches( String("main") ))
   {
      log->out(String("The main flag table cannot be deleted. "), 
          fnname, clname, LogMessage::WARN);
      return False;
   }
   
   /* remove the entry from the list file */
   /* delete the Table associated with it, and set version to main */
   
   ofstream listfile;
   listfile.open(verlistfile_p.data());
   for(Int i=0;i<(Int)versionlist_p.nelements();i++)
   {
      if( ! versionlist_p[i].matches("main") )
      {
         if(versionlist_p[i].matches(versionname))
         {
            log->out(String("Deleting version: ") + versionname, 
                 fnname, clname, LogMessage::WARN);

            String tabvername = flagtablename_p + versionname;
            Table tb;
            if(tb.canDeleteTable(tabvername))
               tb.deleteTable(tabvername);
         }
         else listfile << commentlist_p[i] << endl; 
      }
   }
   listfile.close();

   readVersionList();

   return True;
}
/*********************************************************************************
This function should be changed to not use get(i,...) and put(i, ...) instead
 of getColumn(...) and putColumn(...). Otherwise out of memory errors happen
 for large MSs.
*/
Bool FlagVersion::clearAllFlags()
{
   String fnname= "clearAllFlags";
#if LOG0 
   log->FnEnter(fnname + "()", clname);
#endif 

#if LOG0 
   log->out(String("Clearing All main-table Flags for : ")+tabname_p,
            fnname, clname, LogMessage::DEBUGGING);
#endif 
   
   if(frcol_p)
   {
      ScalarColumn<Bool> rfscol;
      rfscol.attach(tab_p,rowflagcolname_p);
 
      Vector<Bool> frc = rfscol.getColumn();
      frc = False;
      rfscol.putColumn(frc);
   }
   if(fcol_p)
   {
      ArrayColumn<Bool> facol;
      facol.attach(tab_p,dataflagcolname_p);
 
      Array<Bool> fc = facol.getColumn();
      fc = False;
      facol.putColumn(fc);
   }
   
   tab_p.flush();

   return True;
}

/*********************************************************************************/
/* Read in the list of versions from the listfile */
/* Check that flag tables of the correct names exist and have the same
   number of rows as tab_p */
Bool FlagVersion::readVersionList()
{
   String fnname= "readVersionList";
   versionlist_p.resize(1);
   versionlist_p[0] = String("main");
   
   commentlist_p.resize(1);
   commentlist_p[0] = String("main : working copy in main table");

   /* Read the file and make a list of version names */
   ifstream listfile;
   listfile.open(verlistfile_p.data());

   if( !listfile.good() ) 
      log->out("Bad file!", fnname, clname, LogMessage::WARN);

   Int vcount=1;
   char vers[500];
   while(!listfile.eof())
   {
      listfile.getline(vers,500,'\n');
      if(!listfile.eof())
      {
         versionlist_p.resize(vcount+1,True);
         commentlist_p.resize(vcount+1,True);
         versionlist_p[vcount] = String(vers).before(" : ");
         commentlist_p[vcount] = String(vers);
         vcount++;
      }
   }
   listfile.close();
   
   try
   {
      
      /* Try to open each of these flag tables, to check for table validity */
      for(Int i=0;i<(Int)versionlist_p.nelements();i++)
      {
         
         String tabvername = flagtablename_p + versionlist_p[i];
         Table tab;
         if(!versionlist_p[i].matches("main") && ! tab.isReadable(tabvername))
         log->out(String("Flag Table ") + tabvername + 
                 " does not exist. Please check your flag version file",
                 fnname, clname, LogMessage::WARN);
      }
   }
   catch (AipsError x)
   {
      String err = x.getMesg();
      log->out(String("Number of rows in the Flag Tables ") +
           "do not match the number of rows in the main table. " +
           "Please check your selection.\n" + err,
           fnname, clname, LogMessage::SEVERE);
      return False;
   }
   return True;
} /*********************************************************************************/
void FlagVersion::FlagVersionError(String msg)
{
   throw AipsError("FlagVersion: " + msg);
}
/*********************************************************************************/

} //# NAMESPACE CASA - END 

