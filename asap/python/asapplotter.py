from asap.parameters import rcParams
from asap.selector import selector
from asap.scantable import scantable
from asap.logging import asaplog, asaplog_post_dec
import matplotlib.axes
from matplotlib.font_manager import FontProperties
from matplotlib.text import Text

import re

class asapplotter:
    """
    The ASAP plotter.
    By default the plotter is set up to plot polarisations
    'colour stacked' and scantables across panels.

    .. note::

        Currenly it only plots 'spectra' not Tsys or
        other variables.

    """
    def __init__(self, visible=None , **kwargs):
        self._visible = rcParams['plotter.gui']
        if visible is not None:
            self._visible = visible
        self._plotter = self._newplotter(**kwargs)
        # additional tool bar
        self._plotter.figmgr.casabar=self._newcasabar()

        self._panelling = None
        self._stacking = None
        self.set_panelling()
        self.set_stacking()
        self._rows = None
        self._cols = None
        self._autoplot = False
        self._minmaxx = None
        self._minmaxy = None
        self._datamask = None
        self._data = None
        self._lmap = None
        self._title = None
        self._ordinate = None
        self._abcissa = None
        self._abcunit = None
        self._usermask = []
        self._maskselection = None
        self._selection = selector()
        self._hist = rcParams['plotter.histogram']
        self._fp = FontProperties()
        self._margins = self.set_margin(refresh=False)
        self._offset = None
        self._startrow = 0
        self._ipanel = -1
        self._panelrows = []

    def _translate(self, instr):
        keys = "s b i p t r".split()
        if isinstance(instr, str):
            for key in keys:
                if instr.lower().startswith(key):
                    return key
        return None

    def _newplotter(self, **kwargs):
        backend=matplotlib.get_backend()
        if not self._visible:
            from asap.asaplot import asaplot
        elif backend == 'TkAgg':
            from asap.asaplotgui import asaplotgui as asaplot
        elif backend == 'Qt4Agg':
            from asap.asaplotgui_qt4 import asaplotgui as asaplot
        elif backend == 'GTkAgg':
            from asap.asaplotgui_gtk import asaplotgui as asaplot
        else:
            from asap.asaplot import asaplot
        return asaplot(**kwargs)

    def _newcasabar(self):
        backend=matplotlib.get_backend()
        if self._visible and backend == "TkAgg":
            from asap.casatoolbar import CustomToolbarTkAgg
            return CustomToolbarTkAgg(self)
            #from asap.casatoolbar import CustomFlagToolbarTkAgg
            #return CustomFlagToolbarTkAgg(self)
        return None

    @asaplog_post_dec
    def plot(self, scan=None):
        """
        Plot a scantable.
        Parameters:
            scan:   a scantable
        Note:
            If a scantable was specified in a previous call
            to plot, no argument has to be given to 'replot'
            NO checking is done that the abcissas of the scantable
            are consistent e.g. all 'channel' or all 'velocity' etc.
        """
        self._startrow = 0
        self._ipanel = -1
        if self._plotter.is_dead:
            if hasattr(self._plotter.figmgr,'casabar'):
                del self._plotter.figmgr.casabar
            self._plotter = self._newplotter()
            self._plotter.figmgr.casabar=self._newcasabar()
        if self._plotter.figmgr.casabar:
            self._plotter.figmgr.casabar.set_pagecounter(1)
        self._panelrows = []
        self._plotter.hold()
        #self._plotter.clear()
        if not self._data and not scan:
            msg = "Input is not a scantable"
            raise TypeError(msg)
        if scan: 
            self.set_data(scan, refresh=False)
        self._plot(self._data)
        if self._minmaxy is not None:
            self._plotter.set_limits(ylim=self._minmaxy)
        if self._plotter.figmgr.casabar: self._plotter.figmgr.casabar.enable_button()
        self._plotter.release()
        self._plotter.tidy()
        self._plotter.show(hardrefresh=False)
        return

    def gca(self):
        return self._plotter.figure.gca()

    def refresh(self):
        """Do a soft refresh"""
        self._plotter.figure.show()

    def create_mask(self, nwin=1, panel=0, color=None):
        """
        Interactively define a mask. It retruns a mask that is equivalent to
        the one created manually with scantable.create_mask.
        Parameters:
            nwin:       The number of mask windows to create interactively
                        default is 1.
            panel:      Which panel to use for mask selection. This is useful
                        if different IFs are spread over panels (default 0)
        """
        if self._data is None:
            return []
        outmask = []
        self._plotter.subplot(panel)
        xmin, xmax = self._plotter.axes.get_xlim()
        marg = 0.05*(xmax-xmin)
        self._plotter.axes.set_xlim(xmin-marg, xmax+marg)
        self.refresh()

        def cleanup(lines=False, texts=False, refresh=False):
            if lines:
                del self._plotter.axes.lines[-1]
            if texts:
                del self._plotter.axes.texts[-1]
            if refresh:
                self.refresh()

        for w in xrange(nwin):
            wpos = []
            self.text(0.05,1.0, "Add start boundary",
                      coords="relative", fontsize=10)
            point = self._plotter.get_point()
            cleanup(texts=True)
            if point is None:
                continue
            wpos.append(point[0])
            self.axvline(wpos[0], color=color)
            self.text(0.05,1.0, "Add end boundary", coords="relative", fontsize=10)
            point = self._plotter.get_point()
            cleanup(texts=True, lines=True)
            if point is None:
                self.refresh()
                continue
            wpos.append(point[0])
            self.axvspan(wpos[0], wpos[1], alpha=0.1,
                         edgecolor=color, facecolor=color)
            ymin, ymax = self._plotter.axes.get_ylim()
            outmask.append(wpos)

        self._plotter.axes.set_xlim(xmin, xmax)
        self.refresh()
        if len(outmask) > 0:
            return self._data.create_mask(*outmask)
        return []

    # forwards to matplotlib axes
    def text(self, *args, **kwargs):
        if kwargs.has_key("interactive"):
            if kwargs.pop("interactive"):
                pos = self._plotter.get_point()
                args = tuple(pos)+args
        self._axes_callback("text", *args, **kwargs)

    text.__doc__ = matplotlib.axes.Axes.text.__doc__

    def arrow(self, *args, **kwargs):
        if kwargs.has_key("interactive"):
            if kwargs.pop("interactive"):
                pos = self._plotter.get_region()
                dpos = (pos[0][0], pos[0][1],
                        pos[1][0]-pos[0][0],
                        pos[1][1] - pos[0][1])
                args = dpos + args
        self._axes_callback("arrow", *args, **kwargs)

    arrow.__doc__ = matplotlib.axes.Axes.arrow.__doc__

    def annotate(self, text, xy=None, xytext=None, **kwargs):
        if kwargs.has_key("interactive"):
            if kwargs.pop("interactive"):
                xy = self._plotter.get_point()
                xytext = self._plotter.get_point()
        if not kwargs.has_key("arrowprops"):
            kwargs["arrowprops"] = dict(arrowstyle="->")
        self._axes_callback("annotate", text, xy, xytext, **kwargs)

    annotate.__doc__ = matplotlib.axes.Axes.annotate.__doc__

    def axvline(self, *args, **kwargs):
        if kwargs.has_key("interactive"):
            if kwargs.pop("interactive"):
                pos = self._plotter.get_point()
                args = (pos[0],)+args
        self._axes_callback("axvline", *args, **kwargs)

    axvline.__doc__ = matplotlib.axes.Axes.axvline.__doc__

    def axhline(self, *args, **kwargs):
        if kwargs.has_key("interactive"):
            if kwargs.pop("interactive"):
                pos = self._plotter.get_point()
                args = (pos[1],)+args
        self._axes_callback("axhline", *args, **kwargs)

    axhline.__doc__ = matplotlib.axes.Axes.axhline.__doc__

    def axvspan(self, *args, **kwargs):
        if kwargs.has_key("interactive"):
            if kwargs.pop("interactive"):
                pos = self._plotter.get_region()
                dpos = (pos[0][0], pos[1][0])
                args = dpos + args
        self._axes_callback("axvspan", *args, **kwargs)
        # hack to preventy mpl from redrawing the patch
        # it seem to convert the patch into lines on every draw.
        # This doesn't happen in a test script???
        #del self._plotter.axes.patches[-1]

    axvspan.__doc__ = matplotlib.axes.Axes.axvspan.__doc__

    def axhspan(self, *args, **kwargs):
        if kwargs.has_key("interactive"):
            if kwargs.pop("interactive"):
                pos = self._plotter.get_region()
                dpos = (pos[0][1], pos[1][1])
                args = dpos + args
        self._axes_callback("axhspan", *args, **kwargs)
        # hack to preventy mpl from redrawing the patch
        # it seem to convert the patch into lines on every draw.
        # This doesn't happen in a test script???
        #del self._plotter.axes.patches[-1]

    axhspan.__doc__ = matplotlib.axes.Axes.axhspan.__doc__

    def _axes_callback(self, axesfunc, *args, **kwargs):
        panel = 0
        if kwargs.has_key("panel"):
            panel = kwargs.pop("panel")
        coords = None
        if kwargs.has_key("coords"):
            coords = kwargs.pop("coords")
            if coords.lower() == 'world':
                kwargs["transform"] = self._plotter.axes.transData
            elif coords.lower() == 'relative':
                kwargs["transform"] = self._plotter.axes.transAxes
        self._plotter.subplot(panel)
        self._plotter.axes.set_autoscale_on(False)
        getattr(self._plotter.axes, axesfunc)(*args, **kwargs)
        self._plotter.show(False)
        self._plotter.axes.set_autoscale_on(True)
    # end matplotlib.axes fowarding functions

    @asaplog_post_dec
    def set_data(self, scan, refresh=True):
        """
        Set a scantable to plot.
        Parameters:
            scan:      a scantable
            refresh:   True (default) or False. If True, the plot is
                       replotted based on the new parameter setting(s).
                       Otherwise,the parameter(s) are set without replotting.
        Note:
           The user specified masks and data selections will be reset
           if a new scantable is set. This method should be called before
           setting data selections (set_selection) and/or masks (set_mask).
        """
        from asap import scantable
        if isinstance(scan, scantable):
            if self._data is not None:
                if scan != self._data:
                    self._data = scan
                    # reset
                    self._reset()
                    msg = "A new scantable is set to the plotter. "\
                          "The masks and data selections are reset."
                    asaplog.push( msg )
            else:
                self._data = scan
                self._reset()
        else:
            msg = "Input is not a scantable"
            raise TypeError(msg)

        # ranges become invalid when unit changes
        if self._abcunit and self._abcunit != self._data.get_unit():
            self._minmaxx = None
            self._minmaxy = None
            self._abcunit = self._data.get_unit()
            self._datamask = None
        if refresh: self.plot()

    @asaplog_post_dec
    def set_mode(self, stacking=None, panelling=None, refresh=True):
        """
        Set the plots look and feel, i.e. what you want to see on the plot.
        Parameters:
            stacking:     tell the plotter which variable to plot
                          as line colour overlays (default 'pol')
            panelling:    tell the plotter which variable to plot
                          across multiple panels (default 'scan'
            refresh:      True (default) or False. If True, the plot is
                          replotted based on the new parameter setting(s).
                          Otherwise,the parameter(s) are set without replotting.
        Note:
            Valid modes are:
                 'beam' 'Beam' 'b':     Beams
                 'if' 'IF' 'i':         IFs
                 'pol' 'Pol' 'p':       Polarisations
                 'scan' 'Scan' 's':     Scans
                 'time' 'Time' 't':     Times
                 'row' 'Row' 'r':       Rows
            When either 'stacking' or 'panelling' is set to 'row',
            the other parameter setting is ignored.
        """
        msg = "Invalid mode"
        if not self.set_panelling(panelling) or \
               not self.set_stacking(stacking):
            raise TypeError(msg)
        #if self._panelling == 'r':
        #    self._stacking = '_r'
        #if self._stacking == 'r':
        #    self._panelling = '_r'
        if refresh and self._data: self.plot(self._data)
        return

    def set_panelling(self, what=None):
        """Set the 'panelling' mode i.e. which type of spectra should be
        spread across different panels.
        """

        mode = what
        if mode is None:
             mode = rcParams['plotter.panelling']
        md = self._translate(mode)
        if md:
            self._panelling = md
            self._title = None
            #if md == 'r':
            #    self._stacking = '_r'
            # you need to reset counters for multi page plotting
            self._reset_counters()
            return True
        return False

    def set_layout(self,rows=None,cols=None,refresh=True):
        """
        Set the multi-panel layout, i.e. how many rows and columns plots
        are visible.
        Parameters:
             rows:   The number of rows of plots
             cols:   The number of columns of plots
             refresh:  True (default) or False. If True, the plot is
                       replotted based on the new parameter setting(s).
                       Otherwise,the parameter(s) are set without replotting.
        Note:
             If no argument is given, the potter reverts to its auto-plot
             behaviour.
        """
        self._rows = rows
        self._cols = cols
        if refresh and self._data: self.plot(self._data)
        return

    def set_stacking(self, what=None):
        """Set the 'stacking' mode i.e. which type of spectra should be
        overlayed.
        """
        mode = what
        if mode is None:
             mode = rcParams['plotter.stacking']
        md = self._translate(mode)
        if md:
            self._stacking = md
            self._lmap = None
            #if md == 'r':
            #    self._panelling = '_r'
            # you need to reset counters for multi page plotting
            self._reset_counters()
            return True
        return False

    def _reset_counters(self):
        self._startrow = 0
        self._ipanel = -1
        self._panelrows = []

    def set_range(self,xstart=None,xend=None,ystart=None,yend=None,refresh=True, offset=None):
        """
        Set the range of interest on the abcissa of the plot
        Parameters:
            [x,y]start,[x,y]end:  The start and end points of the 'zoom' window
            refresh:  True (default) or False. If True, the plot is
                      replotted based on the new parameter setting(s).
                      Otherwise,the parameter(s) are set without replotting.
            offset:   shift the abcissa by the given amount. The abcissa label will
                      have '(relative)' appended to it.
        Note:
            These become non-sensical when the unit changes.
            use plotter.set_range() without parameters to reset

        """
        self._offset = offset
        if xstart is None and xend is None:
            self._minmaxx = None
        else:
            self._minmaxx = [xstart,xend]
        if ystart is None and yend is None:
            self._minmaxy = None
        else:
            self._minmaxy = [ystart,yend]
        if refresh and self._data: self.plot(self._data)
        return

    def set_legend(self, mp=None, fontsize = None, mode = 0, refresh=True):
        """
        Specify a mapping for the legend instead of using the default
        indices:
        Parameters:
            mp:        a list of 'strings'. This should have the same length
                       as the number of elements on the legend and then maps
                       to the indeces in order. It is possible to uses latex
                       math expression. These have to be enclosed in r'',
                       e.g. r'$x^{2}$'
            fontsize:  The font size of the label (default None)
            mode:      where to display the legend
                       Any other value for loc else disables the legend:
                        0: auto
                        1: upper right
                        2: upper left
                        3: lower left
                        4: lower right
                        5: right
                        6: center left
                        7: center right
                        8: lower center
                        9: upper center
                        10: center
            refresh:    True (default) or False. If True, the plot is
                        replotted based on the new parameter setting(s).
                        Otherwise,the parameter(s) are set without replotting.

        Example:
             If the data has two IFs/rest frequencies with index 0 and 1
             for CO and SiO:
             plotter.set_stacking('i')
             plotter.set_legend(['CO','SiO'])
             plotter.plot()
             plotter.set_legend([r'$^{12}CO$', r'SiO'])
        """
        self._lmap = mp
        self._plotter.legend(mode)
        if isinstance(fontsize, int):
            from matplotlib import rc as rcp
            rcp('legend', fontsize=fontsize)
        if refresh and self._data: self.plot(self._data)
        return

    def set_title(self, title=None, fontsize=None, refresh=True):
        """
        Set the title of the plot. If multiple panels are plotted,
        multiple titles have to be specified.
        Parameters:
            refresh:    True (default) or False. If True, the plot is
                        replotted based on the new parameter setting(s).
                        Otherwise,the parameter(s) are set without replotting.
        Example:
             # two panels are visible on the plotter
             plotter.set_title(["First Panel","Second Panel"])
        """
        self._title = title
        if isinstance(fontsize, int):
            from matplotlib import rc as rcp
            rcp('axes', titlesize=fontsize)
        if refresh and self._data: self.plot(self._data)
        return

    def set_ordinate(self, ordinate=None, fontsize=None, refresh=True):
        """
        Set the y-axis label of the plot. If multiple panels are plotted,
        multiple labels have to be specified.
        Parameters:
            ordinate:    a list of ordinate labels. None (default) let
                         data determine the labels
            refresh:     True (default) or False. If True, the plot is
                         replotted based on the new parameter setting(s).
                         Otherwise,the parameter(s) are set without replotting.
        Example:
             # two panels are visible on the plotter
             plotter.set_ordinate(["First Y-Axis","Second Y-Axis"])
        """
        self._ordinate = ordinate
        if isinstance(fontsize, int):
            from matplotlib import rc as rcp
            rcp('axes', labelsize=fontsize)
            rcp('ytick', labelsize=fontsize)
        if refresh and self._data: self.plot(self._data)
        return

    def set_abcissa(self, abcissa=None, fontsize=None, refresh=True):
        """
        Set the x-axis label of the plot. If multiple panels are plotted,
        multiple labels have to be specified.
        Parameters:
            abcissa:     a list of abcissa labels. None (default) let
                         data determine the labels
            refresh:     True (default) or False. If True, the plot is
                         replotted based on the new parameter setting(s).
                         Otherwise,the parameter(s) are set without replotting.
        Example:
             # two panels are visible on the plotter
             plotter.set_ordinate(["First X-Axis","Second X-Axis"])
        """
        self._abcissa = abcissa
        if isinstance(fontsize, int):
            from matplotlib import rc as rcp
            rcp('axes', labelsize=fontsize)
            rcp('xtick', labelsize=fontsize)
        if refresh and self._data: self.plot(self._data)
        return

    def set_colors(self, colmap, refresh=True):
        """
        Set the colours to be used. The plotter will cycle through
        these colours when lines are overlaid (stacking mode).
        Parameters:
            colmap:     a list of colour names
            refresh:    True (default) or False. If True, the plot is
                        replotted based on the new parameter setting(s).
                        Otherwise,the parameter(s) are set without replotting.
        Example:
             plotter.set_colors("red green blue")
             # If for example four lines are overlaid e.g I Q U V
             # 'I' will be 'red', 'Q' will be 'green', U will be 'blue'
             # and 'V' will be 'red' again.
        """
        if isinstance(colmap,str):
            colmap = colmap.split()
        self._plotter.palette(0, colormap=colmap)
        if refresh and self._data: self.plot(self._data)

    # alias for english speakers
    set_colours = set_colors

    def set_histogram(self, hist=True, linewidth=None, refresh=True):
        """
        Enable/Disable histogram-like plotting.
        Parameters:
            hist:        True (default) or False. The fisrt default
                         is taken from the .asaprc setting
                         plotter.histogram
            refresh:     True (default) or False. If True, the plot is
                         replotted based on the new parameter setting(s).
                         Otherwise,the parameter(s) are set without replotting.
        """
        self._hist = hist
        if isinstance(linewidth, float) or isinstance(linewidth, int):
            from matplotlib import rc as rcp
            rcp('lines', linewidth=linewidth)
        if refresh and self._data: self.plot(self._data)

    def set_linestyles(self, linestyles=None, linewidth=None, refresh=True):
        """
        Set the linestyles to be used. The plotter will cycle through
        these linestyles when lines are overlaid (stacking mode) AND
        only one color has been set.
        Parameters:
             linestyles:     a list of linestyles to use.
                             'line', 'dashed', 'dotted', 'dashdot',
                             'dashdotdot' and 'dashdashdot' are
                             possible
            refresh:         True (default) or False. If True, the plot is
                             replotted based on the new parameter setting(s).
                             Otherwise,the parameter(s) are set without replotting.
        Example:
             plotter.set_colors("black")
             plotter.set_linestyles("line dashed dotted dashdot")
             # If for example four lines are overlaid e.g I Q U V
             # 'I' will be 'solid', 'Q' will be 'dashed',
             # U will be 'dotted' and 'V' will be 'dashdot'.
        """
        if isinstance(linestyles,str):
            linestyles = linestyles.split()
        self._plotter.palette(color=0,linestyle=0,linestyles=linestyles)
        if isinstance(linewidth, float) or isinstance(linewidth, int):
            from matplotlib import rc as rcp
            rcp('lines', linewidth=linewidth)
        if refresh and self._data: self.plot(self._data)

    def set_font(self, refresh=True,**kwargs):
        """
        Set font properties.
        Parameters:
            family:    one of 'sans-serif', 'serif', 'cursive', 'fantasy', 'monospace'
            style:     one of 'normal' (or 'roman'), 'italic'  or 'oblique'
            weight:    one of 'normal or 'bold'
            size:      the 'general' font size, individual elements can be adjusted
                       seperately
            refresh:   True (default) or False. If True, the plot is
                       replotted based on the new parameter setting(s).
                       Otherwise,the parameter(s) are set without replotting.
        """
        from matplotlib import rc as rcp
        fdict = {}
        for k,v in kwargs.iteritems():
            if v:
                fdict[k] = v
        self._fp = FontProperties(**fdict)
        if refresh and self._data: self.plot(self._data)

    def set_margin(self,margin=[],refresh=True):
        """
        Set margins between subplots and plot edges.
        Parameters:
            margin:   a list of margins in figure coordinate (0-1),
                      i.e., fraction of the figure width or height.
                      The order of elements should be:
                      [left, bottom, right, top, horizontal space btw panels,
                      vertical space btw panels].
            refresh:  True (default) or False. If True, the plot is
                      replotted based on the new parameter setting(s).
                      Otherwise,the parameter(s) are set without replotting.
        Note
        * When margin is not specified, the values are reset to the defaults
          of matplotlib.
        * If any element is set to be None, the current value is adopted.
        """
        if margin == []: self._margins=self._reset_margin()
        else:
            self._margins=[None]*6
            self._margins[0:len(margin)]=margin
        #print "panel margin set to ",self._margins
        if refresh and self._data: self.plot(self._data)

    def _reset_margin(self):
        ks=map(lambda x: 'figure.subplot.'+x,
               ['left','bottom','right','top','hspace','wspace'])
        return map(matplotlib.rcParams.get,ks)

    def plot_lines(self, linecat=None, doppler=0.0, deltachan=10, rotate=90.0,
                   location=None):
        """
        Plot a line catalog.
        Parameters:
            linecat:      the linecatalog to plot
            doppler:      the velocity shift to apply to the frequencies
            deltachan:    the number of channels to include each side of the
                          line to determine a local maximum/minimum
            rotate:       the rotation (in degrees) for the text label (default 90.0)
            location:     the location of the line annotation from the 'top',
                          'bottom' or alternate (None - the default)
        Notes:
        If the spectrum is flagged no line will be drawn in that location.
        """
        if not self._data:
            raise RuntimeError("No scantable has been plotted yet.")
        from asap._asap import linecatalog
        if not isinstance(linecat, linecatalog):
            raise ValueError("'linecat' isn't of type linecatalog.")
        if not self._data.get_unit().endswith("Hz"):
            raise RuntimeError("Can only overlay linecatalogs when data is in frequency.")
        from numpy import ma
        for j in range(len(self._plotter.subplots)):
            self._plotter.subplot(j)
            lims = self._plotter.axes.get_xlim()
            for row in range(linecat.nrow()):
                # get_frequency returns MHz
                base = { "GHz": 1000.0, "MHz": 1.0, "Hz": 1.0e-6 }
                restf = linecat.get_frequency(row)/base[self._data.get_unit()]
                c = 299792.458
                freq = restf*(1.0-doppler/c)
                if lims[0] < freq < lims[1]:
                    if location is None:
                        loc = 'bottom'
                        if row%2: loc='top'
                    else: loc = location
                    maxys = []
                    for line in self._plotter.axes.lines:
                        v = line._x
                        asc = v[0] < v[-1]

                        idx = None
                        if not asc:
                            if v[len(v)-1] <= freq <= v[0]:
                                i = len(v)-1
                                while i>=0 and v[i] < freq:
                                    idx = i
                                    i-=1
                        else:
                           if v[0] <= freq <= v[len(v)-1]:
                                i = 0
                                while  i<len(v) and v[i] < freq:
                                    idx = i
                                    i+=1
                        if idx is not None:
                            lower = idx - deltachan
                            upper = idx + deltachan
                            if lower < 0: lower = 0
                            if upper > len(v): upper = len(v)
                            s = slice(lower, upper)
                            y = line._y[s]
                            maxy = ma.maximum(y)
                            if isinstance( maxy, float):
                                maxys.append(maxy)
                    if len(maxys):
                        peak = max(maxys)
                        if peak > self._plotter.axes.get_ylim()[1]:
                            loc = 'bottom'
                    else:
                        continue
                    self._plotter.vline_with_label(freq, peak,
                                                   linecat.get_name(row),
                                                   location=loc, rotate=rotate)
        self._plotter.show(hardrefresh=False)


    def save(self, filename=None, orientation=None, dpi=None):
        """
        Save the plot to a file. The known formats are 'png', 'ps', 'eps'.
        Parameters:
             filename:    The name of the output file. This is optional
                          and autodetects the image format from the file
                          suffix. If non filename is specified a file
                          called 'yyyymmdd_hhmmss.png' is created in the
                          current directory.
             orientation: optional parameter for postscript only (not eps).
                          'landscape', 'portrait' or None (default) are valid.
                          If None is choosen for 'ps' output, the plot is
                          automatically oriented to fill the page.
             dpi:         The dpi of the output non-ps plot
        """
        self._plotter.save(filename,orientation,dpi)
        return

    @asaplog_post_dec
    def set_mask(self, mask=None, selection=None, refresh=True):
        """
        Set a plotting mask for a specific polarization.
        This is useful for masking out "noise" Pangle outside a source.
        Parameters:
             mask:           a mask from scantable.create_mask
             selection:      the spectra to apply the mask to.
             refresh:        True (default) or False. If True, the plot is
                             replotted based on the new parameter setting(s).
                             Otherwise,the parameter(s) are set without replotting.
        Example:
             select = selector()
             select.setpolstrings("Pangle")
             plotter.set_mask(mymask, select)
        """
        if not self._data:
            msg = "Can only set mask after a first call to plot()"
            raise RuntimeError(msg)
        if len(mask):
            if isinstance(mask, list) or isinstance(mask, tuple):
                self._usermask = array(mask)
            else:
                self._usermask = mask
        if mask is None and selection is None:
            self._usermask = []
            self._maskselection = None
        if isinstance(selection, selector):
            self._maskselection = {'b': selection.get_beams(),
                                   's': selection.get_scans(),
                                   'i': selection.get_ifs(),
                                   'p': selection.get_pols(),
                                   't': [] }
        else:
            self._maskselection = None
        if refresh: self.plot(self._data)

    def _slice_indeces(self, data):
        mn = self._minmaxx[0]
        mx = self._minmaxx[1]
        asc = data[0] < data[-1]
        start=0
        end = len(data)-1
        inc = 1
        if not asc:
            start = len(data)-1
            end = 0
            inc = -1
        # find min index
        #while start > 0 and data[start] < mn:
        #    start+= inc
        minind=start
        for ind in xrange(start,end+inc,inc):
            if data[ind] > mn: break
            minind=ind
        # find max index
        #while end > 0 and data[end] > mx:
        #    end-=inc
        #if end > 0: end +=1
        maxind=end
        for ind in xrange(end,start-inc,-inc):
            if data[ind] < mx: break
            maxind=ind
        start=minind
        end=maxind
        if start > end:
            return end,start+1
        elif start < end:
            return start,end+1
        else:
            return start,end

    def _reset(self):
        self._usermask = []
        self._usermaskspectra = None
        self._offset = None
        self.set_selection(None, False)

    def _plot(self, scan):
        savesel = scan.get_selection()
        sel = savesel +  self._selection
        order = self._get_sortstring([self._panelling,self._stacking])
        if order:
            sel.set_order(order)
        scan.set_selection(sel)
        d = {'b': scan.getbeam, 's': scan.getscan,
             'i': scan.getif, 'p': scan.getpol, 't': scan.get_time,
             'r': int}#, '_r': int}

        polmodes = dict(zip(self._selection.get_pols(),
                            self._selection.get_poltypes()))
        # this returns either a tuple of numbers or a length  (ncycles)
        # convert this into lengths
        n0,nstack0 = self._get_selected_n(scan)
        if isinstance(n0, int): n = n0
        else: n = len(n0)
        if isinstance(nstack0, int): nstack = nstack0
        else: nstack = len(nstack0)
        # In case of row stacking
        rowstack = False
        titlemode = self._panelling
        if self._stacking == "r" and self._panelling != "r":
            rowstack = True
            titlemode = '_r'
        nptot = n
        maxpanel, maxstack = 16,16
        if nstack > maxstack:
            msg ="Scan to be overlayed contains more than %d selections.\n" \
                  "Selecting first %d selections..." % (maxstack, maxstack)
            asaplog.push(msg)
            asaplog.post('WARN')
            nstack = min(nstack,maxstack)
        #n = min(n-self._ipanel-1,maxpanel)
        n = n-self._ipanel-1

        ganged = False
        if n > 1:
            ganged = rcParams['plotter.ganged']
            if self._panelling == 'i':
                ganged = False
            if self._rows and self._cols:
                n = min(n,self._rows*self._cols)
                self._plotter.set_panels(rows=self._rows,cols=self._cols,
                                         nplots=n,margin=self._margins,ganged=ganged)
            else:
                n = min(n,maxpanel)
                self._plotter.set_panels(rows=n,cols=0,nplots=n,margin=self._margins,ganged=ganged)
        else:
            self._plotter.set_panels(margin=self._margins)
        #r = 0
        r = self._startrow
        nr = scan.nrow()
        a0,b0 = -1,-1
        allxlim = []
        allylim = []
        #newpanel=True
        newpanel=False
        panelcount,stackcount = 0,0
        # If this is not the first page
        if r > 0:
            # panelling value of the prev page 
            a0 = d[self._panelling](r-1)
            # set the initial stackcount large not to plot
            # the start row automatically
            stackcount = nstack

        while r < nr:
            a = d[self._panelling](r)
            b = d[self._stacking](r)
            if a > a0 and panelcount < n:
                if n > 1:
                    self._plotter.subplot(panelcount)
                self._plotter.palette(0)
                #title
                xlab = self._abcissa and self._abcissa[panelcount] \
                       or scan._getabcissalabel()
                if self._offset and not self._abcissa:
                    xlab += " (relative)"
                ylab = self._ordinate and self._ordinate[panelcount] \
                       or scan._get_ordinate_label()
                self._plotter.set_axes('xlabel', xlab)
                self._plotter.set_axes('ylabel', ylab)
                #lbl = self._get_label(scan, r, self._panelling, self._title)
                lbl = self._get_label(scan, r, titlemode, self._title)
                if isinstance(lbl, list) or isinstance(lbl, tuple):
                    if 0 <= panelcount < len(lbl):
                        lbl = lbl[panelcount]
                    else:
                        # get default label
                        #lbl = self._get_label(scan, r, self._panelling, None)
                        lbl = self._get_label(scan, r, titlemode, None)
                self._plotter.set_axes('title',lbl)
                newpanel = True
                stackcount = 0
                panelcount += 1
                # save the start row to plot this panel for future revisit.
                if self._panelling != 'r' and \
                       len(self._panelrows) < self._ipanel+1+panelcount:
                    self._panelrows += [r]
                    
            #if (b > b0 or newpanel) and stackcount < nstack:
            if stackcount < nstack and (newpanel or rowstack or (a == a0 and b > b0)):
                y = []
                if len(polmodes):
                    y = scan._getspectrum(r, polmodes[scan.getpol(r)])
                else:
                    y = scan._getspectrum(r)
                # flag application
                mr = scan._getflagrow(r)
                from numpy import ma, array
                if mr:
                    y = ma.masked_array(y,mask=mr)
                else:
                    m = scan._getmask(r)
                    from numpy import logical_not, logical_and
                    if self._maskselection and len(self._usermask) == len(m):
                        if d[self._stacking](r) in self._maskselection[self._stacking]:
                            m = logical_and(m, self._usermask)
                    y = ma.masked_array(y,mask=logical_not(array(m,copy=False)))

                x = array(scan._getabcissa(r))
                if self._offset:
                    x += self._offset
                if self._minmaxx is not None:
                    s,e = self._slice_indeces(x)
                    x = x[s:e]
                    y = y[s:e]
                if len(x) > 1024 and rcParams['plotter.decimate']:
                    fac = len(x)/1024
                    x = x[::fac]
                    y = y[::fac]
                llbl = self._get_label(scan, r, self._stacking, self._lmap)
                if isinstance(llbl, list) or isinstance(llbl, tuple):
                    if 0 <= stackcount < len(llbl):
                        # use user label
                        llbl = llbl[stackcount]
                    else:
                        # get default label
                        llbl = self._get_label(scan, r, self._stacking, None)
                self._plotter.set_line(label=llbl)
                plotit = self._plotter.plot
                if self._hist: plotit = self._plotter.hist
                if len(x) > 0 and not mr:
                    plotit(x,y)
                    xlim= self._minmaxx or [min(x),max(x)]
                    allxlim += xlim
                    ylim= self._minmaxy or [ma.minimum(y),ma.maximum(y)]
                    allylim += ylim
                else:
                    xlim = self._minmaxx or []
                    allxlim += xlim
                    ylim= self._minmaxy or []
                    allylim += ylim
                stackcount += 1
                a0=a
                b0=b
                # last in colour stack -> autoscale x
                if stackcount == nstack and len(allxlim) > 0:
                    allxlim.sort()
                    self._plotter.subplots[panelcount-1]['axes'].set_xlim([allxlim[0],allxlim[-1]])
                    if ganged:
                        allxlim = [allxlim[0],allxlim[-1]]
                    else:
                        # clear
                        allxlim =[]

            newpanel = False
            #a0=a
            #b0=b
            # ignore following rows
            if (panelcount == n and stackcount == nstack) or (r == nr-1):
                # last panel -> autoscale y if ganged
                #if rcParams['plotter.ganged'] and len(allylim) > 0:
                if ganged and len(allylim) > 0:
                    allylim.sort()
                    self._plotter.set_limits(ylim=[allylim[0],allylim[-1]])
                break
            r+=1 # next row

        # save the current counter for multi-page plotting
        self._startrow = r+1
        self._ipanel += panelcount
        if self._plotter.figmgr.casabar:
            if self._ipanel >= nptot-1:
                self._plotter.figmgr.casabar.disable_next()
            else:
                self._plotter.figmgr.casabar.enable_next()
            if self._ipanel + 1 - panelcount > 0:
                self._plotter.figmgr.casabar.enable_prev()
            else:
                self._plotter.figmgr.casabar.disable_prev()

        #reset the selector to the scantable's original
        scan.set_selection(savesel)

        #temporary switch-off for older matplotlib
        #if self._fp is not None:
        if self._fp is not None and getattr(self._plotter.figure,'findobj',False):
            for o in self._plotter.figure.findobj(Text):
                o.set_fontproperties(self._fp)

    def _get_sortstring(self, lorders):
        d0 = {'s': 'SCANNO', 'b': 'BEAMNO', 'i':'IFNO',
              'p': 'POLNO', 'c': 'CYCLENO', 't' : 'TIME', 'r':None, '_r':None }
        if not (type(lorders) == list) and not (type(lorders) == tuple):
            return None
        if len(lorders) > 0:
            lsorts = []
            for order in lorders:
                if order == "r":
                    # don't sort if row panelling/stacking
                    return None
                ssort = d0[order]
                if ssort:
                    lsorts.append(ssort)
            return lsorts
        return None

    def set_selection(self, selection=None, refresh=True, **kw):
        """
        Parameters:
            selection:  a selector object (default unset the selection)
            refresh:    True (default) or False. If True, the plot is
                        replotted based on the new parameter setting(s).
                        Otherwise,the parameter(s) are set without replotting.
        """
        if selection is None:
            # reset
            if len(kw) == 0:
                self._selection = selector()
            else:
                # try keywords
                for k in kw:
                    if k not in selector.fields:
                        raise KeyError("Invalid selection key '%s', valid keys are %s" % (k, selector.fields))
                self._selection = selector(**kw)
        elif isinstance(selection, selector):
            self._selection = selection
        else:
            raise TypeError("'selection' is not of type selector")

        order = self._get_sortstring([self._panelling,self._stacking])
        if order:
            self._selection.set_order(order)
        if refresh and self._data: self.plot(self._data)

    def _get_selected_n(self, scan):
        d1 = {'b': scan.getbeamnos, 's': scan.getscannos,
             'i': scan.getifnos, 'p': scan.getpolnos, 't': scan.ncycle,
             'r': scan.nrow}#, '_r': False}
        d2 = { 'b': self._selection.get_beams(),
               's': self._selection.get_scans(),
               'i': self._selection.get_ifs(),
               'p': self._selection.get_pols(),
               't': self._selection.get_cycles(),
               'r': False}#, '_r': 1}
        n =  d2[self._panelling] or d1[self._panelling]()
        nstack = d2[self._stacking] or d1[self._stacking]()
        # handle row panelling/stacking
        if self._panelling == 'r':
            nstack = 1
        elif self._stacking == 'r':
            n = 1
        return n,nstack

    def _get_label(self, scan, row, mode, userlabel=None):
        if isinstance(userlabel, list) and len(userlabel) == 0:
            userlabel = " "
        pms = dict(zip(self._selection.get_pols(),self._selection.get_poltypes()))
        if len(pms):
            poleval = scan._getpollabel(scan.getpol(row),pms[scan.getpol(row)])
        else:
            poleval = scan._getpollabel(scan.getpol(row),scan.poltype())
        d = {'b': "Beam "+str(scan.getbeam(row)),
             #'s': scan._getsourcename(row),
             's': "Scan "+str(scan.getscan(row))+\
                  " ("+str(scan._getsourcename(row))+")",
             'i': "IF"+str(scan.getif(row)),
             'p': poleval,
             't': str(scan.get_time(row)),
             'r': "row "+str(row),
             #'_r': str(scan.get_time(row))+",\nIF"+str(scan.getif(row))+", "+poleval+", Beam"+str(scan.getbeam(row)) }
             '_r': "" }
        return userlabel or d[mode]

    def plotazel(self, scan=None, outfile=None):
        """
        plot azimuth and elevation versus time of a scantable
        """
        visible = rcParams['plotter.gui']
        from matplotlib import pylab as PL
        from matplotlib.dates import DateFormatter, timezone
        from matplotlib.dates import HourLocator, MinuteLocator,SecondLocator, DayLocator
        from matplotlib.ticker import MultipleLocator
        from numpy import array, pi
        if not visible or not self._visible:
            PL.ioff()
            from matplotlib.backends.backend_agg import FigureCanvasAgg
            PL.gcf().canvas.switch_backends(FigureCanvasAgg)
        self._data = scan
        self._outfile = outfile
        dates = self._data.get_time(asdatetime=True)
        t = PL.date2num(dates)
        tz = timezone('UTC')
        PL.cla()
        PL.ioff()
        PL.clf()
        # Adjust subplot margins
        if len(self._margins) != 6:
            self.set_margin(refresh=False)
        lef, bot, rig, top, wsp, hsp = self._margins
        PL.gcf().subplots_adjust(left=lef,bottom=bot,right=rig,top=top,
                                 wspace=wsp,hspace=hsp)

        tdel = max(t) - min(t)
        ax = PL.subplot(2,1,1)
        el = array(self._data.get_elevation())*180./pi
        PL.ylabel('El [deg.]')
        dstr = dates[0].strftime('%Y/%m/%d')
        if tdel > 1.0:
            dstr2 = dates[len(dates)-1].strftime('%Y/%m/%d')
            dstr = dstr + " - " + dstr2
            majloc = DayLocator()
            minloc = HourLocator(range(0,23,12))
            timefmt = DateFormatter("%b%d")
        elif tdel > 24./60.:
            timefmt = DateFormatter('%H:%M')
            majloc = HourLocator()
            minloc = MinuteLocator(30)
        else:
            timefmt = DateFormatter('%H:%M')
            majloc = MinuteLocator(interval=5)
            minloc = SecondLocator(30)

        PL.title(dstr)
        if tdel == 0.0:
            th = (t - PL.floor(t))*24.0
            PL.plot(th,el,'o',markersize=2, markerfacecolor='b', markeredgecolor='b')
        else:
            PL.plot_date(t,el,'o', markersize=2, markerfacecolor='b', markeredgecolor='b',tz=tz)
            #ax.grid(True)
            ax.xaxis.set_major_formatter(timefmt)
            ax.xaxis.set_major_locator(majloc)
            ax.xaxis.set_minor_locator(minloc)
        ax.yaxis.grid(True)
        yloc = MultipleLocator(30)
        ax.set_ylim(0,90)
        ax.yaxis.set_major_locator(yloc)
        if tdel > 1.0:
            labels = ax.get_xticklabels()
        #    PL.setp(labels, fontsize=10, rotation=45)
            PL.setp(labels, fontsize=10)

        # Az plot
        az = array(self._data.get_azimuth())*180./pi
        if min(az) < 0:
            for irow in range(len(az)):
                if az[irow] < 0: az[irow] += 360.0

        ax2 = PL.subplot(2,1,2)
        #PL.xlabel('Time (UT [hour])')
        PL.ylabel('Az [deg.]')
        if tdel == 0.0:
            PL.plot(th,az,'o',markersize=2, markeredgecolor='b',markerfacecolor='b')
        else:
            PL.plot_date(t,az,'o', markersize=2,markeredgecolor='b',markerfacecolor='b',tz=tz)
            ax2.xaxis.set_major_formatter(timefmt)
            ax2.xaxis.set_major_locator(majloc)
            ax2.xaxis.set_minor_locator(minloc)
        #ax2.grid(True)
        ax2.set_ylim(0,360)
        ax2.yaxis.grid(True)
        #hfmt = DateFormatter('%H')
        #hloc = HourLocator()
        yloc = MultipleLocator(60)
        ax2.yaxis.set_major_locator(yloc)
        if tdel > 1.0:
            labels = ax2.get_xticklabels()
            PL.setp(labels, fontsize=10)
            PL.xlabel('Time (UT [day])')
        else:
            PL.xlabel('Time (UT [hour])')

        PL.ion()
        PL.draw()
        if (self._outfile is not None):
           PL.savefig(self._outfile)

    def plotpointing(self, scan=None, outfile=None):
        """
        plot telescope pointings
        """
        visible = rcParams['plotter.gui']
        from matplotlib import pylab as PL
        from numpy import array, pi
        if not visible or not self._visible:
            PL.ioff()
            from matplotlib.backends.backend_agg import FigureCanvasAgg
            PL.gcf().canvas.switch_backends(FigureCanvasAgg)
        self._data = scan
        self._outfile = outfile
        dir = array(self._data.get_directionval()).transpose()
        ra = dir[0]*180./pi
        dec = dir[1]*180./pi
        PL.cla()
        #PL.ioff()
        PL.clf()
        # Adjust subplot margins
        if len(self._margins) != 6:
            self.set_margin(refresh=False)
        lef, bot, rig, top, wsp, hsp = self._margins
        PL.gcf().subplots_adjust(left=lef,bottom=bot,right=rig,top=top,
                                 wspace=wsp,hspace=hsp)
        ax = PL.gca()
        #ax = PL.axes([0.1,0.1,0.8,0.8])
        #ax = PL.axes([0.1,0.1,0.8,0.8])
        ax.set_aspect('equal')
        PL.plot(ra, dec, 'b,')
        PL.xlabel('RA [deg.]')
        PL.ylabel('Declination [deg.]')
        PL.title('Telescope pointings')
        [xmin,xmax,ymin,ymax] = PL.axis()
        PL.axis([xmax,xmin,ymin,ymax])
        #PL.ion()
        PL.draw()
        if (self._outfile is not None):
           PL.savefig(self._outfile)

    # plot total power data
    # plotting in time is not yet implemented..
    @asaplog_post_dec
    def plottp(self, scan=None, outfile=None):
        if self._plotter.is_dead:
            if hasattr(self._plotter.figmgr,'casabar'):
                del self._plotter.figmgr.casabar
            self._plotter = self._newplotter()
            self._plotter.figmgr.casabar=self._newcasabar()
        self._plotter.hold()
        self._plotter.clear()
        from asap import scantable
        if not self._data and not scan:
            msg = "Input is not a scantable"
            raise TypeError(msg)
        if isinstance(scan, scantable):
            if self._data is not None:
                if scan != self._data:
                    self._data = scan
                    # reset
                    self._reset()
            else:
                self._data = scan
                self._reset()
        # ranges become invalid when abcissa changes?
        #if self._abcunit and self._abcunit != self._data.get_unit():
        #    self._minmaxx = None
        #    self._minmaxy = None
        #    self._abcunit = self._data.get_unit()
        #    self._datamask = None

        # Adjust subplot margins
        if len(self._margins) !=6: self.set_margin(refresh=False)
        lef, bot, rig, top, wsp, hsp = self._margins
        self._plotter.figure.subplots_adjust(
            left=lef,bottom=bot,right=rig,top=top,wspace=wsp,hspace=hsp)
        if self._plotter.figmgr.casabar: self._plotter.figmgr.casabar.disable_button()
        self._plottp(self._data)
        if self._minmaxy is not None:
            self._plotter.set_limits(ylim=self._minmaxy)
        self._plotter.release()
        self._plotter.tidy()
        self._plotter.show(hardrefresh=False)
        return

    def _plottp(self,scan):
        """
        private method for plotting total power data
        """
        from numpy import ma, array, arange, logical_not
        r=0
        nr = scan.nrow()
        a0,b0 = -1,-1
        allxlim = []
        allylim = []
        y=[]
        self._plotter.set_panels()
        self._plotter.palette(0)
        #title
        #xlab = self._abcissa and self._abcissa[panelcount] \
        #       or scan._getabcissalabel()
        #ylab = self._ordinate and self._ordinate[panelcount] \
        #       or scan._get_ordinate_label()
        xlab = self._abcissa or 'row number' #or Time
        ylab = self._ordinate or scan._get_ordinate_label()
        self._plotter.set_axes('xlabel',xlab)
        self._plotter.set_axes('ylabel',ylab)
        lbl = self._get_label(scan, r, 's', self._title)
        if isinstance(lbl, list) or isinstance(lbl, tuple):
        #    if 0 <= panelcount < len(lbl):
        #        lbl = lbl[panelcount]
        #    else:
                # get default label
             lbl = self._get_label(scan, r, self._panelling, None)
        self._plotter.set_axes('title',lbl)
        y=array(scan._get_column(scan._getspectrum,-1))
        m = array(scan._get_column(scan._getmask,-1))
        y = ma.masked_array(y,mask=logical_not(array(m,copy=False)))
        x = arange(len(y))
        # try to handle spectral data somewhat...
        l,m = y.shape
        if m > 1:
            y=y.mean(axis=1)
        plotit = self._plotter.plot
        llbl = self._get_label(scan, r, self._stacking, None)
        self._plotter.set_line(label=llbl)
        if len(x) > 0:
            plotit(x,y)


    # forwards to matplotlib.Figure.text
    def figtext(self, *args, **kwargs):
        """
        Add text to figure at location x,y (relative 0-1 coords).
        This method forwards *args and **kwargs to a Matplotlib method,
        matplotlib.Figure.text.
        See the method help for detailed information.
        """
        self._plotter.text(*args, **kwargs)
    # end matplotlib.Figure.text forwarding function


    # printing header information
    @asaplog_post_dec
    def print_header(self, plot=True, fontsize=9, logger=False, selstr='', extrastr=''):
        """
        print data (scantable) header on the plot and/or logger.
        Parameters:
            plot:      whether or not print header info on the plot.
            fontsize:  header font size (valid only plot=True)
            logger:    whether or not print header info on the logger.
            selstr:    additional selection string (not verified)
            extrastr:  additional string to print (not verified)
        """
        if not plot and not logger:
            return
        if not self._data:
            raise RuntimeError("No scantable has been set yet.")
        # Now header will be printed on plot and/or logger.
        # Get header information and format it.
        ssum=self._data.__str__()
        # Print Observation header to the upper-left corner of plot
        if plot:
            headstr=[ssum[ssum.find('Observer:'):ssum.find('Flux Unit:')]]
            headstr.append(ssum[ssum.find('Beams:'):ssum.find('Observer:')]
                         +ssum[ssum.find('Rest Freqs:'):ssum.find('Abcissa:')])
            if extrastr != '': headstr[0]=extrastr+'\n'+headstr[0]
            #headstr[1]='Data File:     '+(filestr or 'unknown')+'\n'+headstr[1]
            ssel='***Selections***\n'+(selstr+self._data.get_selection().__str__() or 'none')
            headstr.append(ssel)
            nstcol=len(headstr)

            self._plotter.hold()
            for i in range(nstcol):
                self._plotter.figure.text(0.03+float(i)/nstcol,0.98,
                             headstr[i],
                             horizontalalignment='left',
                             verticalalignment='top',
                             fontsize=fontsize)
            import time
            self._plotter.figure.text(0.99,0.0,
                            time.strftime("%a %d %b %Y  %H:%M:%S %Z"),
                            horizontalalignment='right',
                            verticalalignment='bottom',fontsize=8)
            self._plotter.release()
            del headstr, ssel
        if logger:
            asaplog.push("----------------\n  Plot Summary\n----------------")
            asaplog.push(extrastr)
            asaplog.push(ssum[ssum.find('Beams:'):])
        del ssum
