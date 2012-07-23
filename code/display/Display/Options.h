//# Options.h: base class for storing and parsing parameters 
//# Copyright (C) 2011
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

#ifndef DISPLAY_OPTIONS_H__
#define DISPLAY_OPTIONS_H__
#include <set>
#include <string>

namespace casa {
    namespace viewer {

	class Options {
	    public:
		class Kernel {
		    public:
			virtual std::string tmp( ) const = 0;
		};

		std::string tmp( ) const { return kernel->tmp( ); }
		std::string temporaryDirectory( const std::string &base_dir_name, bool remove=true );

		Options( ) { }
		virtual ~Options( ) { delete kernel; }

	    private:
		friend class options_init_;
		Options( const Options & ) { }
		const Options &operator=(const Options&) { return *this; }
		void init( Kernel *k ) { kernel = k; }
		std::set<std::string> returned_paths;
		Kernel *kernel;
	};

	extern Options options;

	static class options_init_ {
	    public:
	        options_init_( ) { if ( count++ == 0 ) do_init( ); }
		~options_init_( ) { if ( --count == 0 ) { /* could destruct options */ } }
	    private:
		static unsigned int count;
		// to be defined in qt (or other windowing library) land....
		void do_init( );
	} _options_init_object_;
    }
}

#endif
