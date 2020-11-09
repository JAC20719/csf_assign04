#include <iostream>
#include <stdio.h>
#include <dlfcn.h>
#include <zlib.h>
#include <string.h>
#include <dirent.h>
#include <vector>
#include "image.h"

using std::cout;
using std::endl;
using std::string;
using std::vector;

struct Plugin {
    void *handle;
    const char *(*get_plugin_name)(void);
    const char *(*get_plugin_desc)(void);
    void *(*parse_arguments)(int num_args, char *args[]);
    struct Image *(*transform_image)(struct Image *source, void *arg_data);
};

struct Plugin get_plugin(const char *path) {
  Plugin plugin;
  plugin.handle = dlopen(path, RTLD_LAZY);
  if (!plugin.handle) {
    cout << "Error getting handle" << endl;
    exit(1);
  }
  *(void**) (&plugin.get_plugin_name) = dlsym(plugin.handle, "get_plugin_name");
  if (!plugin.get_plugin_name) {
    cout << "Error getting plugin name func" << endl;
    exit(1);
  }
  *(void**) (&plugin.get_plugin_desc) = dlsym(plugin.handle, "get_plugin_desc");
  if (!plugin.get_plugin_name) {
    cout << "Error getting plugin desc func" << endl;
    exit(1);
  }
  *(void**) (&plugin.parse_arguments) = dlsym(plugin.handle, "parse_arguments");
  if (!plugin.get_plugin_name) {
    cout << "Error getting plugin desc func" << endl;
    exit(1);
  }
  *(void**) (&plugin.transform_image) = dlsym(plugin.handle, "transform_image");
  if (!plugin.get_plugin_name) {
    cout << "Error getting plugin desc func" << endl;
    exit(1);
  }

  return plugin;
}

void list_plugins() {
  DIR *folder;
  struct dirent *entry;
  int files = 0;
  char *nv = getenv("PLUGIN_DIR");
  string path = (nv == NULL) ? "./plugins" : nv;
  
  folder = opendir(const_cast<char*>(path.c_str()));
  vector<string> v; 
  while(entry=readdir(folder)) {
    if ((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)) {
      v.push_back(entry->d_name);
    }
  }
  //v.pop_back();
  //v.pop_back();

  int size = v.size();
  cout << "Loaded " << size << " plugin(s)" << endl;
  for (int i = 0; i < size; i++) {
    string plugin_path = path;
    plugin_path.append("/");
    plugin_path.append(v[i]);
    Plugin plugin = get_plugin(const_cast<char*>(plugin_path.c_str()));
    cout << "   " << plugin.get_plugin_name();
    cout << ": " << plugin.get_plugin_desc() << endl;
  }

  closedir(folder);
}

vector<string> get_plugins() {
  DIR *folder;
  struct dirent *entry;
  int files = 0;
  char *nv = getenv("PLUGIN_DIR");
  string path = (nv == NULL) ? "./plugins" : nv;
  
  folder = opendir(const_cast<char*>(path.c_str()));
  vector<string> v; 
  while(entry=readdir(folder)) {
    if((strcmp(entry->d_name, ".") != 0) && (strcmp(entry->d_name, "..") != 0)) {
      v.push_back(entry->d_name);
    }
  }
  //v.pop_back();
  //v.pop_back();

  v.push_back(path); //for use outside of function, put the path to the plugins at end of vector

  return v;
}

int main(int argc, char *argv[]) {
  if(argc <= 1) {
    cout << "Usage: imgproc <command> [<command args..>]" << endl;
    cout << "Commands are:" << endl;
    cout << "   list" << endl;
    cout << "   exec <plugin> <input img> <output img> [<plugin args..>]" << endl;
    return 0;
  }
  
  if(strcmp(argv[1], "list") == 0) {
    list_plugins();
    return 0;
  }

  if(argc < 5) {
    cout << "Incorrect number of arguments" << endl;
    exit(1);
  }

  if(strcmp(argv[1], "exec") == 0) {
    vector<string> plugins = get_plugins();
    int size = plugins.size() - 1; //minus 1 since path to plugins is at end of vector
    int p = -1; 
    for(int i = 0; i < size; i++) {
      cout << "The abomination: " << plugins[i].substr(0, plugins[i].length() -3) << endl;
      if(strcmp(argv[2], const_cast<char*>((plugins[i].substr(0,plugins[i].length()-3)).c_str())) == 0) {
	p = i;
      }
    }
    if (p == -1) {
      cout << "Plugin does not exist" << endl;
      exit(1);
    }

    string full_path = plugins[size]; //This is the path to the plugins
    full_path.append("/");
    full_path.append(plugins[p]); //This is the plugin
    
    Plugin plugin = get_plugin(const_cast<char*>(full_path.c_str()));

    Image *img = img_read_png(argv[3]);

    cout << "Width: " << img->width << ", Height: " << img->height << endl;

    if (!img) {
      cout << "Image is null :(" << endl;
      exit(1);
    }
    //Get remaining params in a char * []
    int rsize = argc - 5;
    char *rem[rsize];
    for(int i = 0; i < rsize; i++) {
      rem[i] = argv[5 + i];
    }
    Image *t_img = plugin.transform_image(img, plugin.parse_arguments(rsize, rem));
    if (!t_img) {
      cout << "Transformed image is null"<< endl;
      exit(1);
    }
  
    img_write_png(t_img, argv[4]);

    dlclose(plugin.handle);

    return 0;
  }

  return 0;
}

