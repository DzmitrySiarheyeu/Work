/*  Glue functions for the minIni library, based on the FatFs and Petit-FatFs
 *  libraries, see http://elm-chan.org/fsw/ff/00index_e.html
 *
 *  By CompuPhase, 2008-2012
 *  This "glue file" is in the public domain. It is distributed without
 *  warranties or conditions of any kind, either express or implied.
 *
 *  (The FatFs and Petit-FatFs libraries are copyright by ChaN and licensed at
 *  its own terms.)
 */

#define INI_BUFFERSIZE  256       /* maximum line length, maximum path length */

/* You must set _USE_STRFUNC to 1 or 2 in the include file ff.h (or tff.h)
 * to enable the "string functions" fgets() and fputs().
 */
#include "ff.h"
#include "vfs.h"                   /* include tff.h for Tiny-FatFs */

#define INI_FILETYPE    FILE_OBJ
#define ini_openread(filename,file)   (VFS_open((file), (filename), FA_READ+FA_OPEN_EXISTING) == VFS_OK)
#define ini_openwrite(filename,file)  (VFS_open((file), (filename), FA_WRITE+FA_CREATE_ALWAYS) == VFS_OK)
#define ini_close(file)               (VFS_close(file) == VFS_OK)
#define ini_read(buffer,size,file)    f_gets((buffer), (size),((FIL *)((FILE_OBJ *)file)->private_data))
#define ini_write(buffer,file)        f_puts((buffer), ((FIL *)((FILE_OBJ *)file)->private_data))
#define ini_remove(filename)          (VFS_rmdir(filename) == VFS_OK)

#define INI_FILEPOS                   DWORD
#define ini_tell(file,pos)            (*(pos) = f_tell(((FIL *)((FILE_OBJ *)file)->private_data)))
#define ini_seek(file,pos)            (VFS_fseek((file), *(pos)) == VFS_OK)

 #include <string.h>

static int ini_rename(TCHAR *source, const TCHAR *dest)
{
  /* Function f_rename() does not allow drive letters in the destination file */
  const char *drive = strchr(dest, ':');
  drive = (drive == NULL) ? dest : drive + 1;
  return (VFS_rename(source, drive) == FR_OK);
}
