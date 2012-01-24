#ifndef gtkmmtracklist_h
#define gtkmmtracklist_h

// gtkmm headers
#include <gtkmm.h>

// Medusa headers
#include "track.h"

class cGtkmmTrackListSelectedIterator
{
public:
  explicit cGtkmmTrackListSelectedIterator(cGtkmmTrackList& trackList);

  size_t GetCount() const { return selected.size(); }

  bool IsValid() const;

  void Next();

  const Gtk::TreeRow& GetRow();

private:
  cGtkmmTrackList& trackList;

  std::vector<Gtk::TreeModel::Path> selected;

  size_t i;
  size_t n;

  Gtk::TreeModel::const_iterator iter;
};

class cGtkmmMainWindow;

class cGtkmmTrackList : public Gtk::TreeView {
public:
  friend class cGtkmmTrackListSelectedIterator;
  friend class cGtkmmMainWindow;

  explicit cGtkmmTrackList(cGtkmmMainWindow& mainWindow);

  void SetStatePlaying(const cTrack* pTrack);
  void SetStatePaused(const cTrack* pTrack);

  int GetIDFromRow(const Gtk::TreeModel::Row& row) const;

  const cTrack* GetTrackFromRow(const Gtk::TreeModel::Row& row) const;

protected:
  Gtk::Widget& GetWidget() { return playlistScrolledWindow; }

private:
  bool OnSelectFunction(const Glib::RefPtr<Gtk::TreeModel>& model, const Gtk::TreeModel::Path& path, bool path_currently_selected);
  void OnListSelectionChanged();
  bool OnListButtonPressEvent(GdkEventButton* event);
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
  Glib::RefPtr<Gtk::ListStore> playlistTreeModelRef;
  Glib::RefPtr<Gtk::TreeSelection> playlistTreeSelectionRef;
};

#endif // gtkmmtracklist_h
