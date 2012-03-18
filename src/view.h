#ifndef view_h
#define view_h

// Spitfire headers
#include <spitfire/spitfire.h>
#include <spitfire/audio/metadata.h>

namespace medusa
{
  class cController;

  class cView
  {
  public:
    cView();
    virtual ~cView() {}

    void SetController(cController* pController);

    virtual void OnTrackAdded(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData) = 0;
    virtual void OnTracksAdded(const std::vector<cTrack*>& tracks) = 0;

    void Run() { _Run(); }

  protected:
    cController* pController;

  private:
    virtual void _Run() = 0;
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
}

#endif // view_h
