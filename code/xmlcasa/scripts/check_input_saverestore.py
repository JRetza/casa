tasklist = ['accum','bandpass','blcal','browsetable','clean','clearcal','concat','contsub','correct','exportuvfits','feather','flagautocorr','flagdata','flagxy','fluxscale','fringecal','ft','gaincal','imhead','immoments','importvla','importasdm','importuvfits','imsmooth','invert','listhistory','listcal','listobs','listvis','makemask','mosaic','plotants','plotcal','plotms','plotxy','pointcal','polcal','setjy','split','uvmodelfit','viewer', 'widefield']

for task in tasklist:
	print 'task is ',task
	inp(task)
	saveinputs(task)
	taskparameters=task+'.saved'
	execfile(taskparameters)
