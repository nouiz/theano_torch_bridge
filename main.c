#include <stdio.h>
 
#include <lua.h>
#include <lauxlib.h>
#include <lualib.h>
#include <TH.h>
 
static lua_State *create_lua_counter(char *err, size_t errlen)
{
  lua_State *L;
 
  L = luaL_newstate();
  if (L == NULL)
    return NULL;
 
  /* Load the standard Lua libraries. */
  luaL_openlibs(L);
  /* lcounter = require("lcounter")
   * Put the counter module onto the stack. */
  if (luaL_dofile(L, "counter.lua")) {
    snprintf(err, errlen, "Could not load counter module");
    lua_close(L);
    return NULL;
  }
 
  /* Pull new out of the counter module. */
  lua_getfield(L, -1, "new");
 
  /* Verify that new is a function. */
  if (!lua_isfunction(L, -1)) {
    snprintf(err, errlen, "New not a valid function");
    lua_close(L);
    return NULL;
  }

  void * data = malloc(8*8);
  void * data_shape = malloc(1*sizeof(long));
  void * data_strides = malloc(1*sizeof(long));
  ((double *) data)[0] = 8.;
  ((long *) data_shape)[0] = 8;
  ((long *) data_strides)[0] = 8;
  //  make a storage THStorage_(newWithData)(real *data, long size); size in number of element
  printf("%f \n", ((double *) data_shape)[0]);
  THDoubleStorage * storage_ = THDoubleStorage_newWithData(data, 8);
  // make a tensor THTensor_(newWithStorage)(THStorage *storage_, long storageOffset_, THLongStorage *size_, THLongStorage *stride_);
  THLongStorage * size_ = THLongStorage_newWithData(data_shape, 1);
  THLongStorage * stride_ = THLongStorage_newWithData(data_shape, 1);
  THDoubleTensor * new_tensor = THDoubleTensor_newWithStorage(storage_, 0, size_, stride_);
  /* Move the counter module to be the first argument of new. */
  lua_insert(L, -2);

  lua_getfield(L, -1, "print_i");
  //  lua_pushvalue(L, -2);
  luaT_pushudata(L, new_tensor,"torch.DoubleTensor"); //convert from TH to torch object.
  if (lua_pcall(L, 1, 0, 0) != 0) {
    char       err[256];
    snprintf(err, 256, "%s", lua_tostring(L, -1));
    printf("%s\n", err);
    printf("Error calling print_i(c)\n");
    return;
  }


 
  /* Put our actual argument (start) onto the stack. */
  lua_pushinteger(L, 0);
 
  /* Call new(M, start). 2 arguments. 2 return values. */
  if (lua_pcall(L, 2, 2, 0) != 0) {
    snprintf(err, errlen, "%s", lua_tostring(L, -1));
    lua_close(L);
    return NULL;
  }
 
  /* Right now we will either have nil and an error string
   * (nil will be below the string on the stack because it
   * would be returned first and put onto the stack first),
   * or the counter object returned by new. */
  if (lua_type(L, -2) == LUA_TNIL) {
    snprintf(err, errlen, "%s", lua_tostring(L, -1));
    lua_close(L);
    return NULL;
  }
 
  /* Remove the empty filler nil from the top of the stack. The
   * lua_pcall stated 2 return values but on success we only
   * get one so we have nil filler after. */
  lua_pop(L, 1);
 
  if (lua_type(L, -1) != LUA_TTABLE) {
    snprintf(err, errlen, "Invalid type (%d) returned by new", lua_type(L, -1));
    lua_close(L);
    return NULL;
  }
 
  /* We're sure we have a table returned by new.
   * This is the only item on the stack right now. */
  return L;
}
 
static void counter_example(lua_State *L)
{
  int n;
  int isnum;
 
  /* c:add(4)
   * Pull out the add function so we can run it. */
  lua_getfield(L, -1, "add");
  /* Copy (don't move) the counter object so it's the
   * first argument. Meaning we're doing M:add.
   * We want to copy because we want to leave the counter
   * object at the bottom of the stack so we can keep using
   * it. We'll use this pattern for all functions we
   * want to call. */
  lua_pushvalue(L, -2);
  /* Add the argument. */
  lua_pushnumber(L, 4);
  /* Run the function. 2 arguments. No return. */
  if (lua_pcall(L, 2, 0, 0) != 0) {
    printf("Error calling add\n");
    return;
  }
 
  /* c:decrement() */
  lua_getfield(L, -1, "decrement");
  lua_pushvalue(L, -2);
  if (lua_pcall(L, 1, 0, 0) != 0) {
    printf("Error calling decrement\n");
    return;
  }
 
  /* print("val=" .. c:getval()) */
  lua_getfield(L, -1, "getval");
  lua_pushvalue(L, -2);
  if (lua_pcall(L, 1, 1, 0) != 0) {
    printf("Error calling getval\n");
    return;
  }
  n = lua_tointeger(L, -1);//, &isnum);
  /* if (!isnum) { */
  /*   printf("Error getval didn't return a number\n"); */
  /*   return; */
  /* } */
  printf("val=%d\n", n);
  /* Remove the return value from the stack. */
  lua_remove(L, -1);
 
  /* try to print a torch object */ 
  lua_getfield(L, -1, "print_rand");
  //  lua_pushvalue(L, -2);
  if (lua_pcall(L, 0, 1, 0) != 0) {
    char       err[256];
    snprintf(err, 256, "%s", lua_tostring(L, -1));
    printf("%s\n", err);
    printf("Error calling print_rand(c)\n");
    return;
  }
  
  THDoubleTensor *input = luaT_checkudata(L, 2, "torch.DoubleTensor");
  //  THDoubleTensor *input = luaT_checkudata(L, 2, "torch.CudaTensor");
  //THCudaTensor
  double * ptr = THDoubleTensor_data(input);
  ptr[0] += 1;
  printf("%ld\n", THDoubleTensor_nElement(input));
  //THDoubleTensor_

  lua_pop(L, 1);
  lua_getfield(L, -1, "print_i");
  //  lua_pushvalue(L, -2);
  luaT_pushudata(L, input,"torch.DoubleTensor"); //convert from TH to torch object.
  if (lua_pcall(L, 1, 0, 0) != 0) {
    char       err[256];
    snprintf(err, 256, "%s", lua_tostring(L, -1));
    printf("%s\n", err);
    printf("Error calling print_i(c)\n");
    return;
  }


  //pop the tensor from the stack to get it freed.

  //  double * ptr = THDoubleTensor_data(input)
    
  /* c:subtract(-2) */
  lua_getfield(L, -1, "subtract");
  lua_pushvalue(L, -2);
  lua_pushnumber(L, -2);
  if (lua_pcall(L, 2, 0, 0) != 0) {
    printf("Error calling subtract\n");
    return;
  }
 
  /* c:increment() */
  lua_getfield(L, -1, "increment");
  lua_pushvalue(L, -2);
  if (lua_pcall(L, 1, 0, 0) != 0) {
    printf("Error calling increment\n");
    return;
  }
 
  /* print(c) */
  lua_getglobal(L, "print");
  lua_pushvalue(L, -2);
  if (lua_pcall(L, 1, 0, 0) != 0) {
    printf("Error calling print(c)\n");
    return;
  }
 
  /* print(c) (Alternative) */
  printf("%s\n", lua_tostring(L, -1));//, NULL));
  /* luaL_tolstring returns the string and puts it on the
   * stack. This is what the function return is pointing
   * to. We need to remove it from the stack. */
  lua_remove(L, -1);

  /* Right now the only thing on the stack is the counter object. */
}
 
int main(int argc, char **argv)
{
  lua_State *L;
  char       err[256];
 
  L = create_lua_counter(err, sizeof(err));
  if (L == NULL) {
    printf("Error: %s\n", err);
    return 0;
  }
 
  counter_example(L);
 
  lua_close(L);
  return 0;
}
