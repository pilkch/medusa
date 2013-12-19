#ifndef gtkmmtracklist_h
#define gtkmmtracklist_h

// Standard headers
#include <unordered_set>

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
  string_t GetFilePathForRow(const Gtk::TreeModel::Row& row) const;
  string_t GetFilePathForTrackID(trackid_t id) const;
  bool GetPropertiesForRow(const Gtk::TreeModel::Row& row, string_t& sFilePath, spitfire::audio::cMetaData& metaData, TRACK_STATUS& status) const;
  bool GetPropertiesForRow(trackid_t id, string_t& sFilePath, spitfire::audio::cMetaData& metaData, TRACK_STATUS& status) const;

  void SetPropertiesForRow(const Gtk::TreeModel::Row& row, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData, TRACK_STATUS status);
  void SetPropertiesForRow(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData, TRACK_STATUS status);

  void SelectRow(trackid_t id);
  void EnsureRowIsVisible(trackid_t id);

  size_t GetTrackCount() const;
  size_t GetSelectedTrackCount() const;

protected:
  Gtk::Widget& GetWidget() { return playlistScrolledWindow; }

private:
  std::string GetIconFileNameForStatus(TRACK_STATUS status) const;

  void SetIcon(trackid_t id, const std::string& sFile);

  bool OnSelectFunction(const Glib::RefPtr<Gtk::TreeModel>& model, const Gtk::TreeModel::Path& path, bool path_currently_selected);
  void OnListSelectionChanged();
  bool OnListButtonPressEvent(GdkEventButton* event);
  void OnListDoubleClick(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column);
  void OnListSortColumnChanged();

  void AddTrack(trackid_t id, const cTrack& track);

  void DeleteTrack(trackid_t id);
  void DeleteTracks(const std::unordered_set<trackid_t>& tracks);
  void DeleteAll();
  void DeleteAllSelected();

  cGtkmmMainWindow& mainWindow;

  // Tree view
  typedef Glib::RefPtr<Gdk::Pixbuf> cPixbufPtr;
  class cUserData : public Glib::Object {
  public:
    string_t sFilePath;
    spitfire::audio::cMetaData metaData;
    TRACK_STATUS status;
  };
  typedef Glib::RefPtr<cUserData> cUserDataPtr;

  class cModelColumns : public Gtk::TreeModel::ColumnRecord
  {
  public:
    cModelColumns()
    {
      add(id);
      add(pixbuf);
      add(artist);
      add(title);
      add(album);
      add(track);
      add(year);
      add(time);
      add(genre);
      add(comment);
      add(dateAdded);
      add(dateLastPlayed);
      add(filePath);
      add(userdata);
    }

    Gtk::TreeModelColumn<trackid_t> id;
    Gtk::TreeModelColumn<cPixbufPtr> pixbuf;
    Gtk::TreeModelColumn<Glib::ustring> artist;
    Gtk::TreeModelColumn<Glib::ustring> title;
    Gtk::TreeModelColumn<Glib::ustring> album;
    Gtk::TreeModelColumn<Glib::ustring> track;
    Gtk::TreeModelColumn<Glib::ustring> year;
    Gtk::TreeModelColumn<Glib::ustring> time;
    Gtk::TreeModelColumn<Glib::ustring> genre;
    Gtk::TreeModelColumn<Glib::ustring> comment;
    Gtk::TreeModelColumn<Glib::ustring> dateAdded;
    Gtk::TreeModelColumn<Glib::ustring> dateLastPlayed;
    Gtk::TreeModelColumn<Glib::ustring> filePath;
    Gtk::TreeModelColumn<cUserDataPtr> userdata;
  };
  cModelColumns columns;

  Gtk::ScrolledWindow playlistScrolledWindow;
  Glib::RefPtr<Gtk::ListStore> playlistTreeModelRef;
  Glib::RefPtr<Gtk::TreeSelection> playlistTreeSelectionRef;

  size_t nTracks;

  trackid_t idPlayingOrPausedItem;
};
}

#endif // gtkmmtracklist_h
