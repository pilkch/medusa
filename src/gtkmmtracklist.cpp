// Standard headers
#include <iostream>

// Medusa headers
#include "gtkmmmainwindow.h"
#include "gtkmmtracklist.h"
#include "util.h"

namespace medusa
{
// ** cGtkmmTrackListIterator

cGtkmmTrackListIterator::cGtkmmTrackListIterator(cGtkmmTrackList& _trackList) :
  trackList(_trackList),
  i(0),
  n(0),
  iter(_trackList.get_model()->children())
{
  std::cout<<"cGtkmmTrackListIterator::cGtkmmTrackListIterator"<<std::endl;
  n = iter.size();
  std::cout<<"cGtkmmTrackListIterator::cGtkmmTrackListIterator n="<<n<<std::endl;
}

bool cGtkmmTrackListIterator::IsValid() const
{
  return (i < n);
}

void cGtkmmTrackListIterator::Next()
{
  i++;
}

const Gtk::TreeRow& cGtkmmTrackListIterator::GetRow()
{
  ASSERT(IsValid());

  row = iter[i];

  ASSERT(row);
  return row;
}


// ** cGtkmmTrackListSelectedIterator

cGtkmmTrackListSelectedIterator::cGtkmmTrackListSelectedIterator(cGtkmmTrackList& _trackList) :
  trackList(_trackList),
  i(0),
  n(0)
{
  std::cout<<"cGtkmmTrackListSelectedIterator::cGtkmmTrackListSelectedIterator"<<std::endl;
  Glib::RefPtr<Gtk::TreeView::Selection> selectionRef(trackList.get_selection());
  if (selectionRef) {
    std::cout<<"cGtkmmTrackListSelectedIterator::cGtkmmTrackListSelectedIterator Setting selected"<<std::endl;
    selected = selectionRef->get_selected_rows();
  } else std::cout<<"cGtkmmTrackListSelectedIterator::cGtkmmTrackListSelectedIterator Selection was invalid"<<std::endl;

  n = selected.size();
  std::cout<<"cGtkmmTrackListSelectedIterator::cGtkmmTrackListSelectedIterator n="<<n<<std::endl;
}

bool cGtkmmTrackListSelectedIterator::IsValid() const
{
  return (i < n);
}

void cGtkmmTrackListSelectedIterator::Next()
{
  i++;
}

const Gtk::TreeRow& cGtkmmTrackListSelectedIterator::GetRow()
{
  ASSERT(IsValid());

  iter = trackList.get_model()->get_iter(selected[i]);
  ASSERT(iter);
  return *iter;
}


// ** cGtkmmTrackList

cGtkmmTrackList::cGtkmmTrackList(cGtkmmMainWindow& _mainWindow) :
  mainWindow(_mainWindow),
  nTracks(0),
  idPlayingOrPausedItem(INVALID_TRACK)
{
  // Add the TreeView, inside a ScrolledWindow, with the button underneath
  playlistScrolledWindow.add(*this);

  // Only show the scrollbars when they are necessary
  playlistScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  // Create the Tree model:
  playlistTreeModelRef = Gtk::ListStore::create(columns);
  set_model(playlistTreeModelRef);

  // All the items to be reordered with drag-and-drop:
  set_reorderable(true);
  set_rules_hint();
  set_headers_clickable(true);
  set_headers_visible(true);

  // Handle tree selections
  playlistTreeSelectionRef = get_selection();
  playlistTreeSelectionRef->set_mode(Gtk::SELECTION_MULTIPLE);
  playlistTreeSelectionRef->signal_changed().connect(sigc::mem_fun(*this, &cGtkmmTrackList::OnListSelectionChanged));
  playlistTreeSelectionRef->set_select_function(sigc::mem_fun(*this, &cGtkmmTrackList::OnSelectFunction));

  signal_row_activated().connect(sigc::mem_fun(*this, &cGtkmmTrackList::OnListDoubleClick));
  signal_button_press_event().connect(sigc::mem_fun(*this, &cGtkmmTrackList::OnListButtonPressEvent), false);

  // Add the TreeView's view columns:
  append_column(" ", columns.pixbuf);
  append_column("Artist", columns.artist);
  append_column("Title", columns.title);
  append_column("Album", columns.album);
  append_column("Track", columns.track);
  append_column("Year", columns.year);
  append_column("Time", columns.time);
  append_column("Genre", columns.genre);
  append_column("Comment", columns.comment);
  append_column("Date Added", columns.dateAdded);
  append_column("Date Last Played", columns.dateLastPlayed);
  append_column("Path", columns.filePath);

  // Set initial sorting column
  playlistTreeModelRef->set_sort_column(columns.artist, Gtk::SORT_ASCENDING);

  size_t index = 0;

  // Set sorting for each column added
  Gtk::TreeView::Column* pColumn = get_column(index);
  pColumn->set_sort_column(columns.pixbuf);
  index++;

  pColumn = get_column(index);
  pColumn->set_sort_column(columns.artist);
  index++;

  pColumn = get_column(index);
  pColumn->set_sort_column(columns.title);
  index++;

  pColumn = get_column(index);
  pColumn->set_sort_column(columns.album);
  index++;

  pColumn = get_column(index);
  pColumn->set_sort_column(columns.track);
  index++;

  pColumn = get_column(index);
  pColumn->set_sort_column(columns.year);
  index++;

  pColumn = get_column(index);
  pColumn->set_sort_column(columns.time);
  index++;

  pColumn = get_column(index);
  pColumn->set_sort_column(columns.genre);
  index++;

  pColumn = get_column(index);
  pColumn->set_sort_column(columns.comment);
  index++;

  pColumn = get_column(index);
  pColumn->set_sort_column(columns.dateAdded);
  index++;

  pColumn = get_column(index);
  pColumn->set_sort_column(columns.dateLastPlayed);
  index++;

  pColumn = get_column(index);
  pColumn->set_sort_column(columns.filePath);
  index++;
}

trackid_t cGtkmmTrackList::GetTrackIDForRow(const Gtk::TreeModel::Row& row) const
{
  trackid_t id = row[columns.id];

  return id;
}

  string_t cGtkmmTrackList::GetFilePathForRow(const Gtk::TreeModel::Row& row) const
  {
    cUserDataPtr pUserData = row[columns.userdata];
    ASSERT(pUserData);
    return pUserData->sFilePath;
  }

  string_t cGtkmmTrackList::GetFilePathForTrackID(trackid_t id) const
  {
    Gtk::TreeModel::iterator iter = playlistTreeModelRef->children().begin();
    while (iter) {
      Gtk::TreeModel::Row row = *iter;
      if (id == row[columns.id]) return GetFilePathForRow(row);

      iter++;
    }

    return "";
  }

bool cGtkmmTrackList::GetPropertiesForRow(const Gtk::TreeModel::Row& row, string_t& sFilePath, spitfire::audio::cMetaData& metaData, TRACK_STATUS& status) const
{
  cUserDataPtr pUserData = row[columns.userdata];
  if (pUserData) {
    sFilePath = pUserData->sFilePath;
    metaData = pUserData->metaData;
    status = pUserData->status;
    return true;
  }

  return false;
}

bool cGtkmmTrackList::GetPropertiesForRow(trackid_t id, string_t& sFilePath, spitfire::audio::cMetaData& metaData, TRACK_STATUS& status) const
{
  Gtk::TreeModel::iterator iter = playlistTreeModelRef->children().begin();
  while (iter) {
    Gtk::TreeModel::Row row = *iter;
    if (id == row[columns.id]) {
      return GetPropertiesForRow(row, sFilePath, metaData, status);
    }

    iter++;
  }

  return false;
}

void cGtkmmTrackList::SetPropertiesForRow(const Gtk::TreeModel::Row& row, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData, TRACK_STATUS status)
{
  cUserDataPtr pUserData(new cUserData);
  pUserData->sFilePath = sFilePath;
  pUserData->metaData = metaData;
  pUserData->status = status;
  row[columns.userdata]  = pUserData;
}

void cGtkmmTrackList::SetPropertiesForRow(trackid_t id, const string_t& sFilePath, const spitfire::audio::cMetaData& metaData, TRACK_STATUS status)
{
  Gtk::TreeModel::iterator iter = playlistTreeModelRef->children().begin();
  while (iter) {
    Gtk::TreeModel::Row row = *iter;
    if (id == row[columns.id]) {
      SetPropertiesForRow(row, sFilePath, metaData, status);
      break;
    }

    iter++;
  }
}

bool cGtkmmTrackList::OnSelectFunction(const Glib::RefPtr<Gtk::TreeModel>& model, const Gtk::TreeModel::Path& path, bool path_currently_selected)
{
  std::cout<<"cGtkmmTrackList::OnSelectFunction"<<std::endl;

  // Tell Gtk that we want to select the row
  return true;
}

void cGtkmmTrackList::OnListSelectionChanged()
{
  std::cout<<"cGtkmmTrackList::OnListSelectionChanged"<<std::endl;
  mainWindow.OnActionPlaylistSelectionChanged();
}

// Handle right click properly
// 1) Right click on a selected item will not change the selection
// 2) Right click on an unselected item will:
//   2a) Change the selection to the item if 1 or less items were already selected
//   2b) Not change the selection if more than 1 item was already selected
// 3) Handle other events as normal
bool cGtkmmTrackList::OnListButtonPressEvent(GdkEventButton* event)
{
  std::cout<<"cGtkmmTrackList::OnListButtonPressEvent"<<std::endl;

  bool bReturnValue = false;

  // Now show our popup menu
  if ((event->type == GDK_BUTTON_PRESS) && (event->button == 3)) {
    std::cout<<"cGtkmmTrackList::OnListButtonPressEvent Show menu\n";
    Glib::RefPtr<Gtk::TreeView::Selection> selectionRef(get_selection());
    if (selectionRef) {
      if (selectionRef->count_selected_rows() <= 1) {
        // Call base class, to allow normal handling, such as allowing the row to be selected by the right-click
        bReturnValue = TreeView::on_button_press_event(event);
      } else {
        bool bIsSelectedAlready = true;
        // TODO: Iterate through and find if this row is selected yet

        bReturnValue = bIsSelectedAlready;
      }
    } else bReturnValue = TreeView::on_button_press_event(event);

    mainWindow.OnActionPlaylistRightClick(event);
  } else {
    // Call base class, to allow normal handling, such as allowing the row to be selected by the right-click
    bReturnValue = TreeView::on_button_press_event(event);
  }

  return bReturnValue;
}

void cGtkmmTrackList::OnListDoubleClick(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column)
{
  std::cout<<"cGtkmmTrackList::OnListDoubleClick\n";

  Gtk::TreeModel::iterator iter = playlistTreeModelRef->get_iter(path);
  if (iter) {
    Gtk::TreeModel::Row row = *iter;
    std::cout<<"cGtkmmTrackList::OnListDoubleClick ID="<<row[columns.id]<<", "<<row[columns.artist]<<" - "<<row[columns.title]<<std::endl;

    trackid_t id = row[columns.id];

    mainWindow.OnActionPlaylistDoubleClick(id);
  }
}

std::string cGtkmmTrackList::GetIconFileNameForStatus(TRACK_STATUS status) const
{
  if (status == TRACK_STATUS::FILE_DOES_NOT_EXIST) return "file_not_found";
  else if (status == TRACK_STATUS::FILE_EMPTY) return "file_empty";

  return "blank";
}

void cGtkmmTrackList::AddTrack(trackid_t id, const cTrack& track)
{
  std::cout<<"cGtkmmTrackList::AddTrack \""<<track.sFilePath<<"\""<<std::endl;

  Gtk::TreeModel::Row row = *(playlistTreeModelRef->append());
  row[columns.id] = id;
  row[columns.pixbuf] = Gdk::Pixbuf::create_from_file(("data/" + GetIconFileNameForStatus(track.status) + ".xpm").c_str());
  row[columns.artist] = spitfire::string::ToUTF8(track.metaData.sArtist);
  row[columns.title] = spitfire::string::ToUTF8(track.metaData.sTitle);
  row[columns.album] = spitfire::string::ToUTF8(track.metaData.sAlbum);
  row[columns.track] = spitfire::string::ToUTF8(medusa::util::FormatNumber(track.metaData.uiTracknum));
  row[columns.year] = spitfire::string::ToUTF8(medusa::util::FormatNumber(track.metaData.uiYear));
  row[columns.genre] = spitfire::string::ToUTF8(track.metaData.sGenre);
  row[columns.comment] = spitfire::string::ToUTF8(track.metaData.sComment);
  row[columns.time] = spitfire::string::ToUTF8(medusa::util::FormatTime(track.metaData.uiDurationMilliSeconds));
  row[columns.dateAdded] = spitfire::string::ToUTF8(medusa::util::FormatDateTime(track.dateAdded));
  if (!track.dateLastPlayed.IsUnixEpoch()) row[columns.dateLastPlayed] = spitfire::string::ToUTF8(medusa::util::FormatDateTime(track.dateLastPlayed));
  else row[columns.dateLastPlayed] = "";
  row[columns.filePath] = spitfire::string::ToUTF8(track.sFilePath);

  // Custom data
  cUserDataPtr pUserData(new cUserData);
  pUserData->sFilePath = track.sFilePath;
  pUserData->metaData = track.metaData;
  pUserData->status = track.status;
  row[columns.userdata] = pUserData;

  nTracks++;
}

void cGtkmmTrackList::DeleteTrack(trackid_t id)
{
  Gtk::TreeModel::iterator iter = playlistTreeModelRef->children().begin();
  while (iter) {
    Gtk::TreeModel::Row row = *iter;
    if (row[columns.id] == id) {
      playlistTreeModelRef->erase(iter);
      nTracks--;
    }

    iter++;
  }

  // Invalidate our playing or pause item
  if (idPlayingOrPausedItem == id) idPlayingOrPausedItem = INVALID_TRACK;

  OnListSelectionChanged();
}

  void cGtkmmTrackList::DeleteTracks(const std::unordered_set<trackid_t>& tracks)
  {
    Gtk::TreeModel::iterator iter = playlistTreeModelRef->children().begin();
    while (iter) {
      Gtk::TreeModel::Row row = *iter;
      if (tracks.find(row[columns.id]) != tracks.end()) {
        playlistTreeModelRef->erase(iter);
        nTracks--;

        // Invalidate our playing or pause item
        if (idPlayingOrPausedItem == row[columns.id]) idPlayingOrPausedItem = INVALID_TRACK;
      }

      iter++;
    }
  }

void cGtkmmTrackList::DeleteAll()
{
  playlistTreeModelRef.clear();

  nTracks = 0;

  // Invalidate our playing or pause item
  idPlayingOrPausedItem = INVALID_TRACK;

  OnListSelectionChanged();
}

void cGtkmmTrackList::DeleteAllSelected()
{
  // http://www.mail-archive.com/gtkmm-list@gnome.org/msg08840.html
  Glib::RefPtr<Gtk::TreeView::Selection> selectionRef(get_selection());
  if (selectionRef) {
    std::vector<Gtk::TreeModel::Path> selected = selectionRef->get_selected_rows();
    if (selected.size() != 0) {
      // Convert the selected paths to RowReferences
      std::list<Gtk::TreeModel::RowReference> rows;
      for (std::vector<Gtk::TreeModel::Path>::iterator iter = selected.begin(); iter != selected.end(); iter++) {
        rows.push_back(Gtk::TreeModel::RowReference(get_model(), *iter));
      }

      // Remove the rows from the treemodel
      for (std::list<Gtk::TreeModel::RowReference>::iterator iter = rows.begin(); iter != rows.end(); iter++) {
        Gtk::TreeModel::iterator treeiter = playlistTreeModelRef->get_iter(iter->get_path());
        playlistTreeModelRef->erase(treeiter);
        nTracks--;

        // Invalidate our playing or pause item
        Gtk::TreeModel::Row row = *treeiter;
        if (row[columns.id] == idPlayingOrPausedItem) idPlayingOrPausedItem = INVALID_TRACK;
      }
    }
  }

  OnListSelectionChanged();
}

  void cGtkmmTrackList::SetIcon(trackid_t id, const std::string& sFile)
  {
    // Clear the icon for the currently playing or paused item
    if (idPlayingOrPausedItem != INVALID_TRACK) {
      Gtk::TreeModel::iterator iter = playlistTreeModelRef->children().begin();
      while (iter) {
        Gtk::TreeModel::Row row = *iter;
        if (row[columns.id] == idPlayingOrPausedItem) {
          std::string sIconName = "blank";
          cUserDataPtr pUserData = row[columns.userdata];
          if (pUserData) {
            if (pUserData->status != TRACK_STATUS::OK) sIconName = GetIconFileNameForStatus(pUserData->status);
          }
          row[columns.pixbuf] = Gdk::Pixbuf::create_from_file(("data/" + sIconName + ".xpm").c_str());

          break;
        }

        iter++;
      }

      idPlayingOrPausedItem = INVALID_TRACK;
    }

    // Update the icon in the new row
    if (id != INVALID_TRACK) {
      Gtk::TreeModel::iterator iter = playlistTreeModelRef->children().begin();
      while (iter) {
        Gtk::TreeModel::Row row = *iter;
        if (row[columns.id] == id) {
          std::string sIconName = sFile;
          cUserDataPtr pUserData = row[columns.userdata];
          if (pUserData) {
            if (pUserData->status != TRACK_STATUS::OK) sIconName = GetIconFileNameForStatus(pUserData->status);
          }
          row[columns.pixbuf] = Gdk::Pixbuf::create_from_file(("data/" + sIconName + ".xpm").c_str());

          // Update our playing or paused item
          idPlayingOrPausedItem = id;

          break;
        }

        iter++;
      }
    }
  }

void cGtkmmTrackList::SetStatePlaying(trackid_t id)
{
  // Update the icons in the tree view
  SetIcon(id, "playing");
}

void cGtkmmTrackList::SetStatePaused(trackid_t id)
{
  // Update the icons in the tree view
  SetIcon(id, "paused");
}

  size_t cGtkmmTrackList::GetTrackCount() const
  {
    return nTracks;
  }

  size_t cGtkmmTrackList::GetSelectedTrackCount() const
  {
    Glib::RefPtr<const Gtk::TreeView::Selection> selectionRef(get_selection());
    if (selectionRef) return selectionRef->count_selected_rows();

    return 0;
  }

  void cGtkmmTrackList::EnsureRowIsVisible(trackid_t id)
  {
    if (id != INVALID_TRACK) {
      Gtk::TreeModel::iterator iter = playlistTreeModelRef->children().begin();
      while (iter) {
        Gtk::TreeModel::Row row = *iter;
        if (row[columns.id] == id) {
          // Scroll to the requested row
          scroll_to_row(playlistTreeModelRef->get_path(iter));
          break;
        }

        iter++;
      }
    }
  }
}
