#ifndef gtkmmtracklist_h
#define gtkmmtracklist_h

// gtkmm headers
#include <gtkmm.h>

// Medusa headers
#include "track.h"

namespace medusa
{
// ** cGtkmmTrackListIterator

class cGtkmmTrackListIterator
{
public:
  explicit cGtkmmTrackListIterator(cGtkmmTrackList& trackList);

  size_t GetCount() const { return iter.size(); }

  bool IsValid() const;

  void Next();

  const Gtk::TreeRow& GetRow();

private:
  cGtkmmTrackList& trackList;

  size_t i;
  size_t n;

  Gtk::TreeModel::Children iter;
  Gtk::TreeRow row;
};

// ** cGtkmmTrackListSelectedIterator

class cGtkmmTrackListSelectedIterator
{
public:
  explicit cGtkmmTrackListSelectedIterator(cGtkmmTrackList& trackList);

  size_t GetCount() const { return n; }

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

// ** cGtkmmTrackList

class cGtkmmTrackList : public Gtk::TreeView {
public:
  friend class cGtkmmTrackListSelectedIterator;
  friend class cGtkmmMainWindow;

  explicit cGtkmmTrackList(cGtkmmMainWindow& mainWindow);

  void SetStatePlaying(trackid_t id);
  void SetStatePaused(trackid_t id);

  trackid_t GetTrackIDForRow(const Gtk::TreeModel::Row& row) const;
  bool GetPropertiesForRow(const Gtk::TreeModel::Row& row, string_t& sFilePath, spitfire::audio::cMetaData& metaData) const;
  bool GetPropertiesForRow(trackid_t id, string_t& sFilePath, spitfire::audio::cMetaData& metaData) const;

  void SetPropertiesForRow(const Gtk::TreeModel::Row& row, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData);
  void SetPropertiesForRow(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData);

protected:
  Gtk::Widget& GetWidget() { return playlistScrolledWindow; }

private:
  bool OnSelectFunction(const Glib::RefPtr<Gtk::TreeModel>& model, const Gtk::TreeModel::Path& path, bool path_currently_selected);
  void OnListSelectionChanged();
  bool OnListButtonPressEvent(GdkEventButton* event);
  void OnListDoubleClick(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);

  void AddTrack(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData);

  void DeleteAll();
  void DeleteAllSelected();

  cGtkmmMainWindow& mainWindow;

  // Tree view
  typedef Glib::RefPtr<Gdk::Pixbuf> cPixbufPtr;
  class cUserData : public Glib::Object {
  public:
    string_t sFilePath;
    spitfire::audio::cMetaData metaData;
  };
  typedef Glib::RefPtr<cUserData> cUserDataPtr;

  class cModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    cModelColumns()
    { add(id); add(pixbuf); add(artist); add(title); add(album); add(track); add(year); add(time); add(dateAdded); add(filePath); add(userdata); }

    Gtk::TreeModelColumn<trackid_t> id;
    Gtk::TreeModelColumn<cPixbufPtr> pixbuf;
    Gtk::TreeModelColumn<Glib::ustring> artist;
    Gtk::TreeModelColumn<Glib::ustring> title;
    Gtk::TreeModelColumn<Glib::ustring> album;
    Gtk::TreeModelColumn<Glib::ustring> track;
    Gtk::TreeModelColumn<Glib::ustring> year;
    Gtk::TreeModelColumn<Glib::ustring> time;
    Gtk::TreeModelColumn<Glib::ustring> dateAdded;
    Gtk::TreeModelColumn<Glib::ustring> filePath;
    Gtk::TreeModelColumn<cUserDataPtr> userdata;
  };
  cModelColumns columns;

  Gtk::ScrolledWindow playlistScrolledWindow;
  Glib::RefPtr<Gtk::ListStore> playlistTreeModelRef;
  Glib::RefPtr<Gtk::TreeSelection> playlistTreeSelectionRef;
};
}

#endif // gtkmmtracklist_h
