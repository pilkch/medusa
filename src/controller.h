#ifndef MEDUSA_CONTROLLER_H
#define MEDUSA_CONTROLLER_H

namespace medusa
{
  class cController
  {
  public:
    cController(cModel& model, cView& view);

  private:
    cModel& model;
    cView& view;
  };

  // ** cController

  inline cController::cController(cModel& _model, cView& _view) :
    model(_model),
    view(_view)
  {
    model.SetController(this);
    view.SetController(this);
  }
}

#endif // MEDUSA_CONTROLLER_H
