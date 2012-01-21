// Standard headers
#include <iostream>
#include <iomanip>

// Medusa headers
#include "gtkmmview.h"
#include "gtkmmmainwindow.h"

cGtkmmMainWindow::cGtkmmMainWindow(cGtkmmView& _view) :
  view(_view),
  buttonPrevious("Previous"),
  buttonPlay("Play"),
  buttonNext("Next"),
  buttonVolume("Volume"),
  textPosition("0:00"),
  textLength("0:00"),
  dummyCategories("Categories"),
  dummyStatusBar("StatusBar")
{
  set_title("Medusa");
  set_border_width(5);
  set_size_request(400, 300);
  set_default_size(800, 600);

  SetPlaybackLengthMS(1000);

  positionSlider.set_draw_value(false);

  positionSlider.signal_change_value().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlaybackPositionChanged));

  buttonPlay.signal_clicked().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnPlayPauseClicked));

  boxPlaybackButtons.pack_start(*Gtk::manage(new Gtk::Label()), Gtk::PACK_EXPAND_WIDGET);
  boxPlaybackButtons.pack_start(buttonPrevious, Gtk::PACK_SHRINK);
  boxPlaybackButtons.pack_start(buttonPlay, Gtk::PACK_SHRINK);
  boxPlaybackButtons.pack_start(buttonNext, Gtk::PACK_SHRINK);
  boxPlaybackButtons.pack_start(*Gtk::manage(new Gtk::Label()), Gtk::PACK_EXPAND_WIDGET);
  boxPlaybackButtons.pack_start(buttonVolume, Gtk::PACK_SHRINK);

  dummyCategories.set_size_request(150, -1);



  //Add the TreeView, inside a ScrolledWindow, with the button underneath:
  playlistScrolledWindow.add(playlistTreeView);

  //Only show the scrollbars when they are necessary:
  playlistScrolledWindow.set_policy(Gtk::POLICY_AUTOMATIC, Gtk::POLICY_AUTOMATIC);


  boxCategoriesAndPlaylist.pack_start(dummyCategories, Gtk::PACK_SHRINK);
  boxCategoriesAndPlaylist.pack_start(playlistScrolledWindow, Gtk::PACK_EXPAND_WIDGET);

  boxPositionSlider.pack_start(textPosition, Gtk::PACK_SHRINK);
  boxPositionSlider.pack_start(positionSlider, Gtk::PACK_EXPAND_WIDGET);
  boxPositionSlider.pack_start(textLength, Gtk::PACK_SHRINK);

  boxMainWindow.pack_start(boxPlaybackButtons, Gtk::PACK_SHRINK);
  boxMainWindow.pack_start(boxPositionSlider, Gtk::PACK_SHRINK);
  boxMainWindow.pack_start(boxCategoriesAndPlaylist, Gtk::PACK_EXPAND_WIDGET);
  boxMainWindow.pack_start(dummyStatusBar, Gtk::PACK_SHRINK);

  // Add the box layout to the main window
  add(boxMainWindow);





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
  //playlistTreeSelectionRef->signal_changed().connect( sigc::mem_fun(*this, &cGtkmmMainWindow::OnListSelectionChanged) );

  playlistTreeView.signal_row_activated().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnListDoubleClick));

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
  show_all_children();
}

bool cGtkmmMainWindow::OnPlaybackPositionChanged(Gtk::ScrollType scrollType, double value)
{
  std::cout<<"cGtkmmMainWindow::OnPlaybackPositionChanged\n";

  view.OnActionPlaybackPositionChanged(value);

  return true;
}

void cGtkmmMainWindow::OnPlayPauseClicked()
{
  view.OnActionPlayPause();
}

bool cGtkmmMainWindow::OnTimerPlaybackPosition()
{
  view.OnActionTimerUpdatePlaybackPosition();

  return true;
}

void cGtkmmMainWindow::OnListSelectionChanged()
{
  std::cout<<"cGtkmmMainWindow::OnListSelectionChanged\n";
  //m_Button_Delete.set_sensitive(playlistTreeSelectionRef->count_selected_rows() > 0);
}

void cGtkmmMainWindow::OnListDoubleClick(const Gtk::TreeModel::Path& path, Gtk::TreeViewColumn* column)
{
  std::cout<<"cGtkmmMainWindow::OnListDoubleClick\n";

  Gtk::TreeModel::iterator iter = playlistTreeModelRef->get_iter(path);
  if (iter) {
    Gtk::TreeModel::Row row = *iter;
    std::cout<<"cGtkmmMainWindow::OnListDoubleClick ID="<<row[columns.id]<<", "<<row[columns.artist]<<" - "<<row[columns.title]<<std::endl;

    cUserDataPtr pUserData = row[columns.userdata];
    if (pUserData) {
      const cTrack* pTrack = pUserData->pTrack;
      if (pTrack != nullptr) {
        std::wcout<<"cGtkmmMainWindow::OnListDoubleClick Track: "<<pTrack->metaData.sArtist<<" - "<<pTrack->metaData.sTitle<<std::endl;
        view.OnActionPlayTrack(pTrack);
      }
    }
  }
}

void cGtkmmMainWindow::AddTrack(const int id, const cTrack& track)
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

std::string cGtkmmMainWindow::TimeToString(uint64_t milliseconds) const
{
  std::ostringstream o;

  o<<std::right<<std::setfill('0');

  uint64_t time = milliseconds / 1000;

  const uint64_t seconds = time % 60;
  time /= 60;

  const uint64_t minutes = time % 60;
  time /= 60;

  const uint64_t hours = time % 24;
  time /= 24;

  const uint64_t days = hours;

  if (days != 0) {
    o<<days<<":";
    o<<std::setw(2); // Set width of 2 for the next value
  }

  if (hours != 0)  {
    o<<hours<<":";
    o<<std::setw(2); // Set width of 2 for the next value
  }

  o<<minutes<<":";
  o<<std::setw(2)<<seconds;

  return o.str();
}

void cGtkmmMainWindow::SetPlaybackPositionMS(uint64_t milliseconds)
{
  //std::cout<<"cGtkmmMainWindow::SetPlaybackPositionMS "<<milliseconds<<"\n";
  positionSlider.set_value(double(milliseconds) / 1000.0f);

  textPosition.set_text(TimeToString(milliseconds).c_str());
}

void cGtkmmMainWindow::SetPlaybackLengthMS(uint64_t milliseconds)
{
  std::cout<<"cGtkmmMainWindow::SetPlaybackLengthMS "<<milliseconds<<"\n";
  positionSlider.set_range(0, double(milliseconds) / 1000.0f);
  positionSlider.set_increments(5, 5);

  textLength.set_text(TimeToString(milliseconds).c_str());
}

void cGtkmmMainWindow::SetStatePlaying(const cTrack* pTrack)
{
  buttonPlay.set_label("Pause");

  // Update position slider
  SetPlaybackLengthMS(pTrack->metaData.uiDurationMilliSeconds);

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

  // Call OnTimerPlaybackPosition function at a 200ms
  // interval to regularly update the position of the stream
  timeoutConnection = Glib::signal_timeout().connect(sigc::mem_fun(*this, &cGtkmmMainWindow::OnTimerPlaybackPosition), 200);
}

void cGtkmmMainWindow::SetStatePaused()
{
  buttonPlay.set_label("Play");

  // Disconnect the progress signal handler:
  timeoutConnection.disconnect();
}
