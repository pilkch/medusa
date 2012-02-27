#ifndef gtkmmfilebrowse_h
#define gtkmmfilebrowse_h

// Standard headers
#include <string>
#include <vector>

// gtkmm headers
#include <gtkmm.h>

// Spitfire headers
#include <spitfire/util/string.h>

namespace medusa
{
// Browse for a file with file type filtering
//
// cGtkmmFileDialog dialog;
// dialog.SetType(cGtkmmFileDialog::TYPE::OPEN);
// dialog.SetCaption(TEXT("Please choose a file"));
// dialog.SetDefaultFolder(TEXT("/folder"));
//
// cFilter filterText;
// filterText.sName = TEXT("Text files");
// filterText.mimeTypes.push_back("text/plain");
//
// cFilter filterCpp;
// filterCpp.sName = TEXT("C/C++ files");
// filterCpp.mimeTypes.push_back("text/x-c");
// filterCpp.mimeTypes.push_back("text/x-c++");
// filterCpp.mimeTypes.push_back("text/x-c-header");
//
// cFilterList filterList;
// filterList.AddFilter(filterText);
// filterList.AddFilter(filterCpp);
// filterList.AddFilterAllFiles();
//
// if (dialog.Run(*this)) {
//   ... set file (dialog.GetSelectedFile());
// }

// Browse for multiple files
//
// cGtkmmFileDialog dialog;
// dialog.SetType(cGtkmmFileDialog::TYPE::OPEN);
// dialog.SetSelectMultipleFiles(true);
// dialog.SetCaption(TEXT("Please choose some files"));
// dialog.SetDefaultFolder(TEXT("/folder"));
// if (dialog.Run(*this)) {
//   const std::vector<spitfire::string_t>& vSelectedFiles = dialog.GetSelectedFiles();
//   const size_t n = vSelectedFiles.size();
//   for (size_t i = 0; i < n; i++) {
//     ... add file (vSelectedFiles[i])
//   }
// }

// Browse for a Folder
//
// cGtkmmFolderDialog dialog;
// dialog.SetType(cGtkmmFolderDialog::TYPE::SELECT);
// dialog.SetCaption(TEXT("Please choose a folder"));
// dialog.SetDefaultFolder(TEXT("/folder"));
// if (dialog.Run(*this)) {
//   ... set folder (dialog.GetSelectedFolder());
// }


class cFilter
{
public:
  spitfire::string_t sName;
  std::vector<std::string> mimeTypes;
  std::vector<std::string> patterns;
};

class cFilterList
{
public:
  void AddFilter(const cFilter& filter);
  void AddFilterAllFiles();

  std::vector<cFilter> filters;
};

class cGtkmmFileDialog
{
public:
  enum class TYPE {
    OPEN,
    SAVE
  };

  void SetType(TYPE type);
  void SetSelectMultipleFiles(bool bSelectMultipleFiles);
  void SetCaption(const spitfire::string_t& sCaption);
  void SetDefaultFolder(const spitfire::string_t& sDefaultFolder);
  void SetFilterList(const cFilterList& filterList);

  const spitfire::string_t& GetSelectedFile() const;
  const std::vector<spitfire::string_t>& GetSelectedFiles() const;

  bool Run(Gtk::Window& parent);

private:
  TYPE type;
  bool bSelectMultipleFiles;
  spitfire::string_t sCaption;
  spitfire::string_t sDefaultFolder;
  cFilterList filterList;

  spitfire::string_t sSelectedFile;
  std::vector<spitfire::string_t> vSelectedFiles;
};

class cGtkmmFolderDialog
{
public:
  enum class TYPE {
    SELECT,
    SAVE
  };

  void SetType(TYPE type);
  void SetCaption(const spitfire::string_t& sCaption);
  void SetDefaultFolder(const spitfire::string_t& sDefaultFolder);

  const spitfire::string_t& GetSelectedFolder() const;

  bool Run(Gtk::Window& parent);

private:
  TYPE type;
  spitfire::string_t sCaption;
  spitfire::string_t sDefaultFolder;

  spitfire::string_t sSelectedFolder;
};
}

#endif // gtkmmfilebrowse_h
