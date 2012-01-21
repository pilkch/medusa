#ifndef gtkmmmainwindow_h
#define gtkmmmainwindow_h

// gtkmm headers
#include <gtkmm.h>

// Medusa headers
#include "track.h"

class cGtkmmView;

class cGtkmmMainWindow : public Gtk::Window {
public:
  explicit cGtkmmMainWindow(cGtkmmView& view);
  virtual ~cGtkmmMainWindow() {}

  void SetStatePlaying(const cTrack* pTrack);
  void SetStatePaused();
  void SetPlaybackPositionMS(uint64_t milliseconds);
  void SetPlaybackLengthMS(uint64_t milliseconds);

private:
  std::string TimeToString(uint64_t milliseconds) const;

  bool OnPlaybackPositionChanged(Gtk::ScrollType scrollType, double value);
  void OnPlayPauseClicked();
  bool OnTimerPlaybackPosition();
  void OnListSelectionChanged();
  void OnListDoubleClick(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);

  void AddTrack(const int id, const cTrack& track);

  cGtkmmView& view;

  Gtk::VBox boxMainWindow;
  Gtk::HBox boxPlaybackButtons;
  Gtk::HBox boxPositionSlider;
  Gtk::HBox boxCategoriesAndPlaylist;

  Gtk::Button buttonPrevious;
  Gtk::Button buttonPlay;
  Gtk::Button buttonNext;
  Gtk::Button buttonVolume;
  Gtk::Label textPosition;
  Gtk::HScale positionSlider;
  Gtk::Label textLength;

  Gtk::Button dummyCategories;

  Gtk::Label dummyStatusBar;


  // Tree view
  typedef Glib::RefPtr<Gdk::Pixbuf> cPixbufPtr;
  class cUserData : public Glib::Object {
  public:
    const cTrack* pTrack;
  };
  typedef Glib::RefPtr<cUserData> cUserDataPtr;

  class cModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    cModelColumns()
    { add(id); add(pixbuf); add(artist); add(title); add(album); add(userdata); }

    Gtk::TreeModelColumn<int> id;
    Gtk::TreeModelColumn<cPixbufPtr> pixbuf;
    Gtk::TreeModelColumn<Glib::ustring> artist;
    Gtk::TreeModelColumn<Glib::ustring> title;
    Gtk::TreeModelColumn<Glib::ustring> album;
    Gtk::TreeModelColumn<cUserDataPtr> userdata;
  };

  cModelColumns columns;

  Gtk::ScrolledWindow playlistScrolledWindow;
  Gtk::TreeView playlistTreeView;
  Glib::RefPtr<Gtk::ListStore> playlistTreeModelRef;
  Glib::RefPtr<Gtk::TreeSelection> playlistTreeSelectionRef;

  sigc::connection timeoutConnection;
};

#endif // gtkmmmainwindow_h
