#ifndef SKYFS_H
#define SKYFS_H

#ifdef GTA_PS2

#include <rwcore.h>

#ifdef    __cplusplus
extern "C"
{
#endif                          /* __cplusplus */

RwBool SkyInstallFileSystem(void);
void SkySetDirectory(const char *dir);
void SkyRegisterFileOnCd(const char *file);

#ifdef    __cplusplus
}
#endif                          /* __cplusplus */

#endif

#endif // SKYFS_H
