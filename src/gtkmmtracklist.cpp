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
  mainWindow(_mainWindow)
{
  // Add the TreeView, inside a ScrolledWindow, with the button underneath
  playlistScrolledWindow.add(*this);

  // Only show the scrollbars when they are necessary
  playlistScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  // Create the Tree model:
  playlistTreeModelRef = Gtk::ListStore::create(columns);
  set_model(playlistTreeModelRef);

  // All the items to be reordered with drag-and-drop:
  set_reorderable();
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
  append_column("Date Added", columns.dateAdded);
  append_column("Path", columns.filePath);

  // Set initial sorting column
  playlistTreeModelRef->set_sort_column(columns.artist, Gtk::SORT_ASCENDING);

  // Set sorting for each column added
  Gtk::TreeView::Column* pColumn = get_column(0);
  pColumn->set_sort_column(columns.pixbuf);

  pColumn = get_column(1);
  pColumn->set_sort_column(columns.artist);

  pColumn = get_column(2);
  pColumn->set_sort_column(columns.title);

  pColumn = get_column(3);
  pColumn->set_sort_column(columns.album);

  pColumn = get_column(4);
  pColumn->set_sort_column(columns.track);

  pColumn = get_column(5);
  pColumn->set_sort_column(columns.year);

  pColumn = get_column(6);
  pColumn->set_sort_column(columns.time);

  pColumn = get_column(7);
  pColumn->set_sort_column(columns.dateAdded);

  pColumn = get_column(8);
  pColumn->set_sort_column(columns.filePath);
}

const cTrack* cGtkmmTrackList::GetTrackFromRow(const Gtk::TreeModel::Row& row) const
{
  const cTrack* pTrack = nullptr;

  cUserDataPtr pUserData = row[columns.userdata];
  if (pUserData) pTrack = pUserData->pTrack;

  return pTrack;
}

bool cGtkmmTrackList::OnSelectFunction(const Glib::RefPtr<Gtk::TreeModel>& model, const Gtk::TreeModel::Path& path, bool path_currently_selected)
{
  std::cout<<"cGtkmmTrackList::OnSelectFunction"<<std::endl;

  // Tell Gtk that we want to select the row
  return true;
}

void cGtkmmTrackList::OnListSelectionChanged()
{
  std::cout<<"cGtkmmTrackList::OnListSelectionChanged\n";
  //m_Button_Delete.set_sensitive(playlistTreeSelectionRef->count_selected_rows() > 0);
}

// Handle right click properly
// 1) Right click on a selected item will not change the selection
// 2) Right click on an unselected item will:
//   2a) Change the selection to the item if 1 or less items were already selected
//   2b) Not change the selection if more than 1 item was already selected
// 3) Handle other events as normal
bool cGtkmmTrackList::OnListButtonPressEvent(GdkEventButton* event)
{
  std::cout<<"cGtkmmTrackList::OnListButtonPressEvent\n";

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

    cUserDataPtr pUserData = row[columns.userdata];
    if (pUserData) {
      const cTrack* pTrack = pUserData->pTrack;
      if (pTrack != nullptr) {
        std::wcout<<"cGtkmmTrackList::OnListDoubleClick Track: "<<pTrack->metaData.sArtist<<" - "<<pTrack->metaData.sTitle<<std::endl;
        mainWindow.OnActionPlayTrack(pTrack);
      }
    }
  }
}

void cGtkmmTrackList::AddTrack(const int id, const cTrack& track)
{
  Gtk::TreeModel::Row row = *(playlistTreeModelRef->append());
  row[columns.id] = id;
  row[columns.pixbuf] = Gdk::Pixbuf::create_from_file("empty.xpm");
  row[columns.artist] = spitfire::string::ToUTF8(track.metaData.sArtist);
  row[columns.title] = spitfire::string::ToUTF8(track.metaData.sTitle);
  row[columns.album] = spitfire::string::ToUTF8(track.metaData.sAlbum);
  row[columns.track] = spitfire::string::ToUTF8(medusa::util::FormatNumber(track.metaData.uiTracknum));
  row[columns.year] = spitfire::string::ToUTF8(medusa::util::FormatNumber(track.metaData.uiYear));
  row[columns.time] = spitfire::string::ToUTF8(medusa::util::FormatTime(track.metaData.uiDurationMilliSeconds));
  row[columns.dateAdded] = "2012/02/28"; //spitfire::string::ToUTF8(medusa::util::FormatTime(track.ui));
  row[columns.filePath] = spitfire::string::ToUTF8(track.sFilePath);

  // Custom data
  cUserDataPtr pUserData(new cUserData);
  pUserData->pTrack = &track;
  row[columns.userdata]  = pUserData;
}

void cGtkmmTrackList::SetStatePlaying(const cTrack* pTrack)
{
  // Update the icons in the tree view
  if (pTrack != nullptr) {
    Gtk::TreeModel::iterator iter = playlistTreeModelRef->children().begin();
    while (iter) {
      Gtk::TreeModel::Row row = *iter;
      cUserDataPtr pUserData = row[columns.userdata];
      if (pUserData) {
        const cTrack* pRowTrack = pUserData->pTrack;
        row[columns.pixbuf] = Gdk::Pixbuf::create_from_file((pRowTrack == pTrack) ? "playing.xpm" : "empty.xpm");
      }

      iter++;
    }
  }
}

void cGtkmmTrackList::SetStatePaused(const cTrack* pTrack)
{
  // Update the icons in the tree view
  if (pTrack != nullptr) {
    Gtk::TreeModel::iterator iter = playlistTreeModelRef->children().begin();
    while (iter) {
      Gtk::TreeModel::Row row = *iter;
      cUserDataPtr pUserData = row[columns.userdata];
      if (pUserData) {
        const cTrack* pRowTrack = pUserData->pTrack;
        row[columns.pixbuf] = Gdk::Pixbuf::create_from_file((pRowTrack == pTrack) ? "paused.xpm" : "empty.xpm");
      }

      iter++;
    }
  }
}
}
