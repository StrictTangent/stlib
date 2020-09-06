
#ifndef __INCLUDE_STLIB_H__
#define __INCLUDE_STLIB_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <inttypes.h>

/*********************************************************
st_arr
Dynamically resizing array.

Initialize like this:

type * myarray = NULL;

THEN, st_arr_push(myarray, <somevalue>)
OR    st_arr_add(myarray, <somevalue>)
OR    st_arr_new(myarray, <initialcapacity>)

If you start using the array without using st_arr_new()
to specify an intial capacity, the capacity will be set
to ST_ARR_DEFAULT_SIZE.

The array can be indexed like:

myarr[<index_value>];

...but storing information beyond the current end of the
array via indexing will not automatically expand the length.
Data stored this way may be lost.

*********************************************************/

#define ST_ARR_DEFAULT_SIZE 100

typedef struct {
  int length;	// Current Used Length
  int size;	// Total Allocation
} st_arr_info;

void * st__arr_new(int size, int count);
void * st_arr_grow(void * ptr, int size, int amount);
void * st__arr_addn(void * ptr, int size, int count);
void * st__arr_copy(void * ptr, int size);

void st__arr_trim(void ** ptr, int size);
void st_arr_free(void * ptr);

int st__arr_deleten(void * ptr, int size, int i, int n);
int st__arr_insertn(void ** ptr, int size, int i, int n);
int st__arr_len(void * ptr);

#define st_arr_new(a, c) (a = st__arr_new(sizeof(*a), c))
#define st_arr_len(a) (a ? st__arr_len(a) : 0)
#define st_arr_head(a) (((st_arr_info *) a) - 1)

#define st_arr_addn(a, n) (a = st__arr_addn(a, sizeof(*a), n), a + st_arr_len(a) - n)
#define st_arr_add(a) (st_arr_addn(a, 1))
#define st_arr_push(a, v) (*st_arr_add(a) = v)
#define st_arr_pop(a) (a[--st_arr_head(a)->length])

#define st_arr_insertn(a, i, n) (st__arr_insertn((void **) &a, sizeof(*a), i, n))
#define st_arr_insert(a, i, v) (st_arr_insertn(a, i, 1), a[i] = v)
#define st_arr_deleten(a, i, n) (st__arr_deleten(a, sizeof(*a), i, n))
#define st_arr_delete(a, i) (st_arr_deleten(a, i, 1))

#define st_arr_printsize(a) (st__arr_printsize((a), sizeof(*a)))
#define st_arr_for(v, a) for(v = a; v < a + st_arr_len(a); v++)
#define st_arr_isempty(a) (st_arr_head(a)>length == 0)
#define st_arr_trim(a) (st__arr_trim((void** )&a, sizeof(*a)))
#define st_arr_destroy(a) (st_arr_free(a))

#define st_arr_copy(a) (st__arr_copy(a, sizeof(*a)))

void st__arr_printsize(void * ptr, int size){
  printf("pointer size is: %ld\n", sizeof(ptr));
  printf("%d\n", size);
}

// Allocates space for new array + info header
// Returns pointer to location where the actual array begins
void * st__arr_new(int size, int count){
  if (count <= 0) count = ST_ARR_DEFAULT_SIZE;
  size = size * count;
  size += sizeof(st_arr_info);
  void * ret = malloc(size);
  
  st_arr_info * info = ret;
  info->length = 0;
  info->size = count;
  ret = ((st_arr_info *) ret) + 1; 
  return ret;

}

// Returns length of array (up to current "end")
int st__arr_len(void * ptr){
  if (!ptr) return 0;
  st_arr_info * info = ((st_arr_info *) ptr) - 1;
  return info->length;
}

// Increases amount of memory allocated to the array
// Returns pointer to location where the actual array begins
void * st_arr_grow(void * ptr, int size, int amount){
  int totalsize = (size * amount) + sizeof(st_arr_info);
  st_arr_info * head = ((st_arr_info *) ptr) - 1;
  head = realloc(head, totalsize);
  head->size = amount;
  ptr = head + 1;
  return ptr;
}

// Increases the size of the array, growing the allocation if necessary
// Returns pointer to location where the actual array begins
void * st__arr_addn(void * ptr, int size, int count){
  if (!ptr){
    ptr = st__arr_new(size, ST_ARR_DEFAULT_SIZE);
  }
  st_arr_info * info = ((st_arr_info *) ptr) - 1;
  if (info->size < info->length + count){
    int totalsize = info->size + (info->size / 2);
    if (totalsize < info->length + count){
      totalsize = info->length + count;
    }
    info = st_arr_grow(ptr, size, totalsize);
    info = info - 1;
  }
  ptr = info + 1;
  info->length += count;
  return ptr;
}

// Reduces memory allocation to fit current length
// of the array exactly.
void st__arr_trim(void ** ptr, int size){
  void * arr = *ptr;
  if (st_arr_head(arr)->size > st_arr_head(arr)->length){
    int newsize = st_arr_head(arr)->length * size;
    newsize += sizeof(st_arr_info);
    st_arr_info * head = st_arr_head(arr);
    head = (st_arr_info *) realloc(head, newsize);
    head->size = head->length;
    *ptr = head + 1;
  }
}

// Frees the entire structure (header, plus array)
void st_arr_free(void * ptr){
  free(st_arr_head(ptr));
}

// Removes n elements from array beginning at index i
// Returns 0 if successful
int st__arr_deleten(void * ptr, int size, int i, int n){
  if (i >= st_arr_head(ptr)->length){
    fprintf(stderr, "st_arr_delete: index out of bounds\n");
    return -1;
  }
  if (i + n > st_arr_head(ptr)->length){
    fprintf(stderr, "st_arr_deleten: deletion range out of bounds\n");
    return -1;
  }
  void * dest = ((char *) ptr) + (size * i);
  void * src = ((char *) dest) + (size * n);
  int amount = (st__arr_len(ptr) - (i+n)) * size;
  memmove(dest, src, amount);
  st_arr_head(ptr)->length -= n;
  return 0;
}

// Inserts n elements into array beginning at index i
// Returns 0 if succesful
int st__arr_insertn(void ** ptr, int size, int i, int n){
  void * arr = *ptr;
  int arrsize;
  if (!arr){
    if (i == 0) {
      arr = st__arr_new(size, i + n);
      st_arr_head(arr)->length = i + n;
      *ptr = arr;
      return 0;
    } else {
      fprintf(stderr, "st_arr_insert: index out of bounds\n");
      return -1;
    }
  }
  if (i > st_arr_head(arr)->length){
    fprintf(stderr, "st_arr_insert: index out of bounds\n");
    return -1;
  } else {
    if (st_arr_head(arr)->length + n > st_arr_head(arr)->size){
      arrsize = st_arr_head(arr)->length + n;;
      arr = st_arr_grow(arr, size, arrsize);
    }
    void * src =  ((char *) arr) + (size * i);
    void * dest = ((char *) src) + (size * n);
    int amount = (st__arr_len(arr) - i) * size;  
    memmove(dest, src, amount);
    st_arr_head(arr)->length += n;
    *ptr = arr;
  }
  return 0;
}

// Creates a copy of the entire structure
// Returns pointer to beginning of the array
void * st__arr_copy(void * ptr, int size){
  int totalsize = st_arr_head(ptr)->size * size;
  totalsize += sizeof(st_arr_info);
  st_arr_info * newcopy = malloc(totalsize);
  memcpy(newcopy, st_arr_head(ptr), totalsize);
  return newcopy + 1;
}




/************************************************
                    st_map

Initialize like this:

st_map mymap = st_map_createmap(<initial_array_size>, <size_of_element>);

Free like this:

st_map_destroy(mymap);

=====================================================

     Add to the map using pointers or values:

Pointers Example:

int * intptr;
*intptr = 5;
st_map_add(mymap, "key", intptr);

Value Example:

int myint = 5;
st_map_addval(mymap, "key", myint);
OR
st_map_add(mymap, "key", &myint);

=====================================================

      Retrieve copys of, or pointers to, 
      elements in the map.

Copy:

mytype * myptr = malloc(sizeof(mytype));
st_map_getcopy(mymap, "key", myptr);

OR with variables instead of pointers:
mytype myval;
st_map_getcopy(mymap, "key", &myval);

Pointer:

mytype * myptr;
st_map_getptr(mymap, "key", myptr);

NOTE: In both these caes, the function returns 0 if
      the key is indeed in the map. To get a pointer
      more directly use:

mytype * myptr = (mytype *) st_map_getptr_direct(mymap, "key");

NOTE: Becareful when using pointers to elements in
      the map. Modifying their contents modifies 
      data maintained by the map itself. While this
      may often be convenient, it is safer to modify
      copies and then re-add them to the map.

=====================================================

Get a list of keys:
st_map_keys(mymap);

Check if key in map:
st_map_inmap(mymap, "key");

**************************************************/

struct st_map_node {
  char * key;
  void * value;
  struct st_map_node * next;
};

#define FNV_PRIME 1099511628211;
#define FNV_OFFSET UINT64_C(14695981039346656037);

typedef struct{
  struct st_map_node ** arr;
  int size;		// total size of array
  int valuesize;	// size of data in bytes
  int count;		// number of elements stored
  int max;		// max possible size of array
  int rehashing;	// 1 if currently rehashing
  char ** keys;
} st_map;

st_map * st_map_createmap(int size, int valuesize);
struct st_map_node * st__map_createnode(char * key, void * value, int valuesize);

int st_map_add(st_map * hmap, char * key, void * value);
int st__map_get(st_map * hmap, char * key, void **retptr);

int st__map_hash(char * str, int size);
void st__map_rehash(st_map * hmap);

char ** st_map_keys(st_map * hmap);
int st_map_inmap(st_map * hmap, char * key);
void st_map_destroy(st_map *hmap);

#define st_map_getptr(m, k, v) (st__map_get(m, k, (void **) &v ) )
#define st_map_addval(m, k, v) (st_map_add(m, k, &v))


// Creates allocations for a hashmap using
// values of given valuesize
// Returns pointer to st_map
st_map * st_map_createmap(int size, int valuesize){
  st_map * ret = malloc(sizeof(st_map));
  ret->arr = calloc(size, sizeof(struct st_map_node *));
  ret->size = size;
  ret->valuesize = valuesize;
  ret->count = 0;
  ret->max = INT_MAX;
  ret->rehashing = 0;
  ret->keys = NULL;
  return ret;
}



// I originally used this hash function found here:
// https://twpower.github.io/160-hash-table-implementation-in-cpp-en
// Very few collissions verses FNVa, but overall slower.
int st__map_hash_orig(char * str, int size){
  int hash = 401;

  while(*str != 0){
    hash = ((hash << 4) + (int)(*str)) % size;
    str++;
  }
  return hash % size;
}

//FNVa Hash
int st__map_hash(char * str, int size){

  unsigned long long hash = FNV_OFFSET;

  while (*str != 0){
    hash ^= (unsigned long long) (*str); // xor
    str++;
    hash *= FNV_PRIME;
  }
  
  return hash % size;
}

// Double the size of the array and rehash
void st__map_rehash(st_map * hmap){
  int oldsize = hmap->size;
  hmap->size = hmap->size * 2;
  hmap->count = 0;
  
  struct st_map_node ** oldarr = hmap->arr;
  hmap->arr = calloc(hmap->size, sizeof(struct st_map_node **));

  struct st_map_node * curr;
  struct st_map_node * temp;

  for (int i = 0; i < oldsize; i++){
    curr = oldarr[i];
    while(curr){
      st_map_add(hmap, curr->key, curr->value);
      temp = curr;
      curr = curr->next;
      free(temp->key);
      free(temp->value);
      free(temp);     
    }
  }
  free(oldarr);
}

// Creates and returns new node containing copy of the data
// stored at memory location "value"
struct st_map_node * st__map_createnode(char * key, void * value, int valuesize){
  struct st_map_node * newnode = malloc(sizeof(struct st_map_node));
  newnode->key = malloc( strlen(key) + 1);
  strcpy(newnode->key, key);
  newnode->value = malloc(valuesize);
  memcpy(newnode->value, value, valuesize);
  newnode->next = NULL;
  return newnode;
}

// Adds a copy of the data stored at location "value"
// to the hashmap with given "key"
int st_map_add(st_map * hmap, char * key, void * value){
  if (hmap->count == hmap->size && !hmap->rehashing){
    hmap->rehashing = 1;
    st__map_rehash(hmap);
    hmap->rehashing = 0;
  }
 
  int pos = st__map_hash(key, hmap->size);

  struct st_map_node * head = hmap->arr[pos];  
 
  if (head == NULL){
    hmap->arr[pos] = st__map_createnode(key, value, hmap->valuesize);
    hmap->count++;
  } else {
    /*
    if (!hmap->rehashing){
      c_count++;
      fprintf(stderr, "COLLISSION @ %d, count: %d\n", pos, c_count);
    }*/
    while(head){
      if (!strcmp(head->key, key)){
	memcpy(head->value, value, hmap->valuesize);
	return 0;
      }
      if (!head->next) break;
      head = head->next;     
    }
    head->next = st__map_createnode(key, value, hmap->valuesize);    
    hmap->count++;  
  } 
  return 0;
  
}
// Stores copy of pointer to value associated
// with key into retptr
// Returns 0 if successful
int st__map_get(st_map * hmap, char * key, void **retptr){
  int pos = st__map_hash(key, hmap->size);

  struct st_map_node * curr = hmap->arr[pos];
  
  while (curr){
    if (!strcmp(curr->key, key)){
      *retptr = curr->value;
      return 0;
    } else {
      curr = curr->next;
    }
  }
  return 1;  
}

// Copies value associated with key
// Into memory location retptr
// Space MUST already be allocated at retptr
// Returns 0 if successful
int st_map_getcopy(st_map * hmap, char * key, void *retptr){
  int pos = st__map_hash(key, hmap->size);
  
  struct st_map_node * curr = hmap->arr[pos];
  
  while (curr){
    if (!strcmp(curr->key, key)){
      memcpy(retptr, curr->value, hmap->valuesize);
      return 0;
    } else {
      curr = curr->next;
    }
  }
  return 1;  
}

// Simply returns pointer to the value associated with key
void * st_map_getptr_direct(st_map * hmap, char * key){
  void * buffer;
  if (st_map_getptr(hmap, key, buffer)) return NULL;
  return buffer;
}

// Removes node containing key/value pair
void st_map_remove(st_map * hmap, char * key){
  int pos = st__map_hash(key, hmap->size);

  struct st_map_node * curr = hmap->arr[pos];
  struct st_map_node * prev = hmap->arr[pos];

  while(curr){
    // if found...
    if (!strcmp(curr->key, key)){
      if(prev != curr) {
	prev->next = curr->next;
      } else {
	hmap->arr[pos] = curr->next;
      }
      hmap->count = hmap->count - 1;
      free(curr->key);
      free(curr->value);
      free(curr);
      
      return;
    } else {
      prev = curr;
      curr = curr->next;
    }
  }
  
}

// Returns 1 if node with matching key is in the map
// Returns 0 otherwise
int st_map_inmap(st_map * hmap, char * key){
  int pos = st__map_hash(key, hmap->size);
  if (!hmap->arr[pos]) return 0;
  struct st_map_node * curr = hmap->arr[pos];
  while (curr){
    if (!strcmp(curr->key, key)) return 1;
    curr = curr->next;
  }
  return 0;
}

// Returns a char * array containing all the keys
char ** st_map_keys(st_map * hmap){
  char ** keys = realloc(hmap->keys, sizeof(char *) * (hmap->count + 1));
  keys[hmap->count] = NULL;
  int k = 0;
  struct st_map_node * curr;
  for (int i = 0; i < hmap->size; i++){
    curr = hmap->arr[i];
    while (curr){
      keys[k] = curr->key;
      k++;
      curr = curr->next;
    }
  }
  hmap->keys = keys;
  return keys;
}

// Frees all memory allocated for the hashmap and its contents
void st_map_destroy(st_map *hmap){
  free(hmap->keys);
  struct st_map_node * curr;
  struct st_map_node * next;
  for (int i = 0; i < hmap->size; i++){
    curr = hmap->arr[i];
    while (curr){
      next = curr->next;
      
      free(curr->key);
      free(curr->value);
      free(curr);
      
      curr = next;
    }
  }
  free(hmap->arr);
  free(hmap);
}

  
#endif


