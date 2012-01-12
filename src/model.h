#ifndef model_h
#define model_h

class cController;

class cModel
{
public:
  cModel();

  void SetController(cController* pController);

private:
  cController* pController;
};

// ** cModel

inline cModel::cModel() :
  pController(nullptr)
{
}

inline void cModel::SetController(cController* _pController)
{
  pController = _pController;
}

#endif // model_h
