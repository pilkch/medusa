// Standard headers
#include <iostream>
#include <iomanip>

// Medusa headers
#include "gtkmmmainwindow.h"
#include "gtkmmtracklist.h"

cGtkmmTrackList::cGtkmmTrackList(cGtkmmMainWindow& _mainWindow) :
  mainWindow(_mainWindow)
{
  // Add the TreeView, inside a ScrolledWindow, with the button underneath
  playlistScrolledWindow.add(playlistTreeView);

  // Only show the scrollbars when they are necessary
  playlistScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);

  // Create the Tree model:
  playlistTreeModelRef = Gtk::ListStore::create(columns);
  playlistTreeView.set_model(playlistTreeModelRef);

  // All the items to be reordered with drag-and-drop:
  playlistTreeView.set_reorderable();
  playlistTreeView.set_rules_hint();
  playlistTreeView.set_headers_clickable(true);
  playlistTreeView.set_headers_visible(true);

  // Handle tree selections
  playlistTreeSelectionRef = playlistTreeView.get_selection();
  //playlistTreeSelectionRef->signal_changed().connect( sigc::mem_fun(*this, &cGtkmmTrackList::OnListSelectionChanged) );

  playlistTreeView.signal_row_activated().connect(sigc::mem_fun(*this, &cGtkmmTrackList::OnListDoubleClick));

  // Add the TreeView's view columns:
  playlistTreeView.append_column(" ", columns.pixbuf);
  playlistTreeView.append_column("Artist", columns.artist);
  playlistTreeView.append_column("Title", columns.title);
  playlistTreeView.append_column("Album", columns.album);

  // Set initial sorting column
  playlistTreeModelRef->set_sort_column(columns.artist, Gtk::SORT_ASCENDING);

  // Set sorting for each column added
  Gtk::TreeView::Column* pColumn = playlistTreeView.get_column(0);
  pColumn->set_sort_column(columns.pixbuf);

  pColumn = playlistTreeView.get_column(1);
  pColumn->set_sort_column(columns.artist);

  pColumn = playlistTreeView.get_column(2);
  pColumn->set_sort_column(columns.title);

  pColumn = playlistTreeView.get_column(3);
  pColumn->set_sort_column(columns.album);
}

void cGtkmmTrackList::OnListSelectionChanged()
{
  std::cout<<"cGtkmmTrackList::OnListSelectionChanged\n";
  //m_Button_Delete.set_sensitive(playlistTreeSelectionRef->count_selected_rows() > 0);
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
