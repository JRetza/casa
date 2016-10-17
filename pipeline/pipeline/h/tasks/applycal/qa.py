from __future__ import absolute_import
import pipeline.qa.scorecalculator as qacalc
import pipeline.infrastructure.logging as logging
import pipeline.infrastructure.pipelineqa as pqa
import pipeline.infrastructure.utils as utils

from . import applycal

LOG = logging.get_logger(__name__)


class ApplycalQAHandler(pqa.QAResultHandler):    
    result_cls = applycal.ApplycalResults
    child_cls = None

    def handle(self, context, result):
        vis = result.inputs['vis']
        ms = context.observing_run.get_ms(vis)
    
        # calculate QA scores from agentflagger summary dictionary, adopting
        # the minimum score as the representative score for this task
        try:
            scores = [qacalc.score_applycal_agents(ms, result.summaries)]
        except:
            scores = [pqa.QAScore(1.0,longmsg='Flag Summary off', shortmsg='Flag Summary off')]
        
        result.qa.pool[:] = scores


class ApplycalListQAHandler(pqa.QAResultHandler):
    result_cls = list
    child_cls = applycal.ApplycalResults

    def handle(self, context, result):
        # collate the QAScores from each child result, pulling them into our
        # own QAscore list
        collated = utils.flatten([r.qa.pool for r in result]) 
        result.qa.pool[:] = collated
