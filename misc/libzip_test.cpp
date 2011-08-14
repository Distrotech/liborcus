#include <zip.h>

#include <iostream>
#include <string>

using namespace std;

int main(int argc, const char** argv)
{
    if (argc < 2)
    {
        return 1;
    }
    cout << "opening " << argv[1] << endl;

    int error;
    struct zip* archive = zip_open(argv[1], 0, &error);
    if (!archive)
    {
        cout << "failed with the error code " << error << endl;
        return 1;
    }

    zip_uint64_t num = zip_get_num_entries(archive, ZIP_FL_UNCHANGED);
    cout << "number of files this archive contains: " << num << endl;

    for (zip_uint64_t i = 0; i < num; ++i)
    {
        const char* filename = zip_get_name(archive, i, ZIP_FL_UNCHANGED);
        cout << filename << endl;
    }

    struct zip_stat file_stat;
    if (zip_stat(archive, "[Content_Types].xml", 0, &file_stat))
    {
        cout << "failed to get stat on [Content_Types].xml" << endl;
        return 1;
    }

    cout << "name: " << file_stat.name << "  size: " << file_stat.size << endl;
    struct zip_file* zfd = zip_fopen(archive, file_stat.name, ZIP_FL_UNCHANGED);
    if (zfd)
    {
        string buf(static_cast<size_t>(file_stat.size), 0);
        int buf_read = zip_fread(zfd, &buf[0], file_stat.size);
        cout << "actual buffer read: " << buf_read << endl;
        cout << buf << endl;
        zip_fclose(zfd);
    }

    zip_close(archive);
    return 0;
}
