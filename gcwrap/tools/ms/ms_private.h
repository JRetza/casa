casa::Bool detached();
casa::Bool ready2write_();

casa::MeasurementSet *itsMS, *itsOriginalMS;
casa::MSSelection *itsMSS;
casa::LogIO *itsLog;
casa::MSSelector *itsSel;
casa::MSFlagger *itsFlag;
casa::VisibilityIterator *itsVI;
casa::VisBuffer *itsVB;

void addephemcol(const casa::MeasurementSet& appendedMS);
