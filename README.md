# stlib - StrictTangent Library
Helpful structures and functions for C programming

This is a personal library for functions I thought useful.\
Currently included are implementations for dynamic arrays and generic hash maps.

## Dynamic Array - st_arr 

Dynamically resizing array which may be indexed like normal arrays


#### Initialize:
```c
mytype * myarray = NULL;
```
#### Adding Elements:
```c
st_arr_push(myarray, <somevalue>); // Puts value at end of the array
st_arr_add(myarray); // Increases array length by 1 
st_arr_addn(myarray, <somevalue>); // Increases array length by <somevalue>
st_arr_new(myarray, <initialcapacity>); // Sets up new array
```

If you start using the array without using st_arr_new() to specify an intial capacity, the capacity will be set to ST_ARR_DEFAULT_SIZE.

#### Index:
```c
myarr[<index_value>];
```
Storing information beyond the current end of the array via indexing will not automatically expand the length. Data stored this way may be lost.

#### Pop the last element off the list:
```c
st_arr_pop(myarray);
```
#### Iterate throught all elements with st_arr_for:
Example using array of doubles:
```c
double * myarray;
st_arr_push(myarray, 1.0);
st_arr_push(myarray, 2.0);
st_arr_push(myarray, 3.0);
double * i;
st_arr_for(i, myarray){
  printf("%f\n, *i);
}
```
Output:
```
1.0
2.0
3.0
```



## Generic Hash Map - st_map

Create a hash map with basically anything by using void pointers. 

#### Initialize:
```c
st_map mymap = st_map_createmap(<initial_array_size>, <size_of_element>);
```
#### Free:
```c
st_map_destroy(mymap);
```

#### st_map_add - Add key/value pairs to the map using pointers or values:\\
Pointers Example:
```c
int * intptr;
*intptr = 5;
st_map_add(mymap, "key", intptr); // Copies value stored at memory address
```
Value Example:
```c
int myint = 5;
st_map_addval(mymap, "key", myint); // Copies variable

// Or...
st_map_add(mymap, "key", &myint); // Copies value stored at memory address
```
#### getcopy - Retrieve copy of value in the map:
```c
mytype * myptr = malloc(sizeof(mytype));
st_map_getcopy(mymap, "key", myptr); // Copies value into memory location myptr
```
OR with variables instead of pointers:
```c
mytype myval;
st_map_getcopy(mymap, "key", &myval); // Copies value into myval
```
#### st_map_getptr - Retrieve pointer to value in the map:
```c
mytype * myptr;
st_map_getptr(mymap, "key", myptr); // Just saves pointer to value stored in the map
```
#### NOTE: 
In both these cases, the function returns 0 if the key is indeed in the map. To get a pointer more directly use:
```c
mytype * myptr = (mytype *) st_map_getptr_direct(mymap, "key"); // saves pointer to value stored in map
```
#### NOTE: 
Be careful when using pointers to elements in the map. Modifying their contents modifies data maintained by the map itself. While this may often be convenient, it is safer to modify copies and then re-add them to the map.

=====================================================

Get a list of keys:
```c
char ** keylist = st_map_keys(mymap);
```
Check if key in map:
```c
st_map_inmap(mymap, "key");
```
