<%!
rsc_path = ""
import os
import pipeline.infrastructure.renderer.rendererutils as rendererutils
%>
<%inherit file="t2-4m_details-base.html"/>
<%block name="header" />

<%block name="title">Bandpass Calibration</%block>

<script>
$(document).ready(function() {
    // return a function that sets the Ant text field to the given ant
    var createAntSetter = function(ant) {
        return function() {
			$("#select-ant").select2("val", [ant]).trigger("change");
        };
    };

    // create a callback function for each overview plot that will select the
    // appropriate spw once the page has loaded
    $(".thumbnail a").each(function (i, v) {
        var o = $(v);
        var ant = o.data("ant");
        o.data("callback", createAntSetter(ant));
    });

    $(".fancybox").fancybox({
        type: 'image',
        prevEffect: 'none',
        nextEffect: 'none',
        loop: false,
        helpers: {
            title: {
                type: 'outside'
            },
            thumbs: {
                width: 50,
                height: 50,
            }
        }
    });
    
    Holder.run();
});
</script>


<p>This task creates bandpass solutions for each measurement set.</p>

<h2>Results</h2>

%if phaseup_applications:
<h4>Phase-up on bandpass calibrator</h4>
<table class="table table-bordered" summary="Application Results">
	<caption>Applied calibrations and parameters used for phase-up calibration</caption>
    <thead>
        <tr>
            <th scope="col" rowspan="2">Measurement Set</th>
			<th scope="col" colspan="5">Phase-up Solution Parameters</th>
<%doc>
            <th scope="col" rowspan="2">% Flagged</th>
</%doc>
		</tr>
		<tr>
			<th>Type</th>
            <th>Interval</th>
			<th>Min Baselines per Antenna</th>
			<th>Min SNR</th>
			<th>Phase-up Bandwidth</th>
        </tr>
    </thead>
	<tbody>
% for application in phaseup_applications:
		<tr>
			<td>${application.ms}</td>
		  	<td>${application.calmode}</td>
		  	<td>${application.solint}</td>
		  	<td>${application.minblperant}</td>
		  	<td>${application.minsnr}</td>
		  	<td>${application.phaseupbw}</td>
<%doc>
		  	<td>${application.flagged}</td>
</%doc>
		</tr>
% endfor		
	</tbody>
</table>

<h4>Bandpass calibration</h4>
%endif

<table class="table table-bordered" summary="Application Results">
	<caption>Parameters used for bandpass calibration</caption>
    <thead>
        <tr>
            <th scope="col" rowspan="2">Measurement Set</th>
			<th scope="col" colspan="2">Solution Parameters</th>
			<th scope="col" colspan="2">Applied To</th>
            <th scope="col" rowspan="2">Calibration Table</th>
		</tr>
		<tr>
			<th>Type</th>
            <th>Interval</th>
			<th>Scan Intent</th>
			<th>Spectral Windows</th>
        </tr>
    </thead>
	<tbody>
	% for tr in bandpass_table_rows:
		<tr>
		% for td in tr:
			${td}
		% endfor
		</tr>
	%endfor
	</tbody>
</table>

% if amp_refant or amp_mode:
<h2>Plots</h2>

<p>Plots show the bandpass correction applied to the target source. 
The first two plots show amplitude vs frequency; one for the reference antenna
and one for a typical antenna, identified the antenna with mode score.
The third plot shows phase vs frequency for the typical antenna.
</p>

<p>Click the summary plots to enlarge them, or the plot title to see
see detailed plots per spectral window and antenna.</p> 

% for ms in amp_refant:		

<h4>${ms}</h4>
% if amp_refant[ms] or amp_mode[ms]:
<div class="col-md-8">
	<div class="thumbnail">
		<div class="caption">
			<h4>Amplitude vs frequency 
				(<a class="replace"
		             href="${os.path.relpath(os.path.join(dirname, amp_subpages[ms]), pcontext.report_dir)}">show all</a>)
			</h4>
			<p>The plots below show amplitude vs frequency for the
			bandpass correction, overlayed for all spectral windows
            and correlations. Click on the link above to show
            show detailed plots for all antennas, or on the links
            below to show plots with specific antennas preselected.</p>
		</div>		
		<div class="row">
	        <% 
	        	plot = amp_refant[ms]
	        %>
			<div class="col-md-6">
		        % if plot is None or not os.path.exists(plot.thumbnail):
		        <img data-src="holder.js/255x188/text:Not Available">	
				<div class="caption">
					<h5>Reference antenna</h5>
                    <p>The amplitude vs frequency plot for the reference antenna 
                    is not available.</p>
				</div>
		        % else:
				<a href="${os.path.relpath(plot.abspath, pcontext.report_dir)}"
                   class="fancybox"
                   rel="plots-${ms}">
					<img src="${os.path.relpath(plot.thumbnail, pcontext.report_dir)}"
                         title="Click to show amplitude vs time plot"
                         data-thumbnail="${os.path.relpath(plot.thumbnail, pcontext.report_dir)}">
				</a>
				<div class="caption">
					<h5>
		                Reference antenna (${plot.parameters['ant']})
	                    (<a href="${os.path.relpath(os.path.join(dirname, amp_subpages[ms]), pcontext.report_dir)}"
	                       class="replace"
	                       data-ant="${plot.parameters['ant']}">
	                       show ${plot.parameters['ant']}</a>)
					</h5>
                    <p>Amplitude vs frequency for the reference antenna 
                    (${plot.parameters['ant']}). Click the link above to show 
                    detailed plots for ${plot.parameters['ant']}.</p>
				</div>
				% endif
			</div>

	        <% 
	        	plot = amp_mode[ms]
	        %>
			<div class="col-md-6">
		        % if plot is None or not os.path.exists(plot.thumbnail):
		        <img data-src="holder.js/255x188/text:Not Available">	
				<div class="caption">
					<h5>Typical antenna</h5>
                    <p>The amplitude vs frequency plot for a typical antenna 
                    is not available.</p>
				</div>
		        % else:
				<a href="${os.path.relpath(plot.abspath, pcontext.report_dir)}"
                   class="fancybox"
                   rel="plots-${ms}">
					<img src="${os.path.relpath(plot.thumbnail, pcontext.report_dir)}"
                         title="Click to show amplitude vs time plot"
                         data-thumbnail="${os.path.relpath(plot.thumbnail, pcontext.report_dir)}">
				</a>
				<div class="caption">
					<h5>Typical antenna (${plot.parameters['ant']})
	                    (<a href="${os.path.relpath(os.path.join(dirname, amp_subpages[ms]), pcontext.report_dir)}"
	                       class="replace"
	                       data-ant="${plot.parameters['ant']}">
	                       show ${plot.parameters['ant']}
	                    </a>)
					</h5>
                    <p>Amplitude vs frequency for a typical antenna 
                    (${plot.parameters['ant']}). Click the link above to show
                    detailed plots for ${plot.parameters['ant']}.</p>
                    <p>NB. random antenna until scores are working</p>
				</div>
				% endif
			</div>
		</div>
	</div>
</div>
% endif

% if phase_mode[ms]:
<div class="col-md-4">
	<div class="thumbnail">
		<div class="caption">
			<h4>Phase vs frequency 
				(<a class="replace"
		            href="${os.path.relpath(os.path.join(dirname, phase_subpages[ms]), pcontext.report_dir)}">show all</a>)
			</h4>
			<p>The plot below shows phase vs frequency for the
			bandpass correction, overlayed for all spectral windows
			and correlations. Click on the link above to show
			show phase vs frequency plots for all antennas, or on the
			link for just the reference antenna.</p>
		</div>		
        <% 
        	plot = phase_refant[ms]
        %>
		<div class="row">
			<div class="col-md-12">
		        % if plot is None or not os.path.exists(plot.thumbnail):
		        <img data-src="holder.js/255x188/text:Not Available">	
				<div class="caption">
					<h5>Reference Antenna</h5>
					<p>The phase vs frequency plot for the reference antenna 
					is not available.</p>
				</div>
		        % else:
				<a href="${os.path.relpath(plot.abspath, pcontext.report_dir)}"
	                  class="fancybox"
	                  rel="plots-${ms}">
					<img src="${os.path.relpath(plot.thumbnail, pcontext.report_dir)}"
	                        title="Click to show phase vs frequency plot"
	                        data-thumbnail="${os.path.relpath(plot.thumbnail, pcontext.report_dir)}">
				</a>
				<div class="caption">
					<h5>Reference antenna (${plot.parameters['ant']})
	                    (<a href="${os.path.relpath(os.path.join(dirname, phase_subpages[ms]), pcontext.report_dir)}"
	                       class="replace"
	                       data-ant="${plot.parameters['ant']}">
	                       show ${plot.parameters['ant']}
	                    </a>)
					</h5>
					<p>Phase vs frequency for the reference antenna 
					(${plot.parameters['ant']}). Click the link above to show
					detailed plots for ${plot.parameters['ant']}.</p>
				</div>
				% endif
			</div>
		</div>
	</div>
</div>
% endif

% endfor		

% endif
