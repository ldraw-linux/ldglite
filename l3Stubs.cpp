
#include <stdio.h>
#include <stdlib.h>

#include "L3Defs.h"

void          CCLoadModelPre(void)
  {
    LoadModelPre();
  }
int           CCLoadModel(const char *lpszPathName)
  {
    return LoadModel(lpszPathName);
  }
void          CCLoadModelPost(void)
  {
    LoadModelPost();
  }

"C" {
void          LoadModelPre(void)
  {
    CCLoadModelPre();
  }
int           LoadModel(const char *lpszPathName)
  {
    return CCLoadModel(lpszPathName);
  }
void          LoadModelPost(void)
  {
    CCLoadModelPost();
  }
}

