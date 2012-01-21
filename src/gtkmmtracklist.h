#ifndef gtkmmtracklist_h
#define gtkmmtracklist_h

// gtkmm headers
#include <gtkmm.h>

// Medusa headers
#include "track.h"

class cGtkmmMainWindow;

class cGtkmmTrackList : public Gtk::TreeView {
public:
  friend class cGtkmmMainWindow;

  explicit cGtkmmTrackList(cGtkmmMainWindow& mainWindow);
  virtual ~cGtkmmTrackList() {}

  void SetStatePlaying(const cTrack* pTrack);
  void SetStatePaused(const cTrack* pTrack);

protected:
  Gtk::Widget& GetWidget() { return playlistScrolledWindow; }

private:
  void OnListSelectionChanged();
  void OnListDoubleClick(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);

  void AddTrack(const int id, const cTrack& track);

  cGtkmmMainWindow& mainWindow;

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
};

#endif // gtkmmtracklist_h
