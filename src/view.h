#ifndef view_h
#define view_h

// TODO: INCLUDE SPITFIRE.H INSTEAD
#define nullptr NULL

class cController;

class cView
{
public:
  cView();
  virtual ~cView() {}

  void SetController(cController* pController);

  void Run() { _Run(); }

private:
  virtual void _Run() = 0;

  cController* pController;
};

// ** cView

inline cView::cView() :
  pController(nullptr)
{
}

inline void cView::SetController(cController* _pController)
{
  pController = _pController;
}

#endif // view_h
