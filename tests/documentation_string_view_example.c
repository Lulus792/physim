/* Appended to the exact published C block from docs/string-view.md. */
#include <stdio.h>
#include <string.h>
int main(void) {
    char input[]=" radius = 0.1 ";
    ps_string_view key={0},value={0};
    if(split_setting((ps_string_view){input,strlen(input)},&key,&value)!=PS_OK)return 1;
    if(key.data!=input+1 || key.size!=6 || memcmp(key.data,"radius",6) ||
       value.data!=input+10 || value.size!=3 || memcmp(value.data,"0.1",3))return 1;
    if(strcmp(input," radius = 0.1 "))return 1;
    ps_string_view saved_key=key,saved_value=value;
    if(split_setting((ps_string_view){"missing",7},&key,&value)!=PS_EOF ||
       key.data!=saved_key.data || key.size!=saved_key.size ||
       value.data!=saved_value.data || value.size!=saved_value.size)return 1;
    if(split_setting((ps_string_view){" = 1",4},&key,&value)!=PS_INVALID ||
       key.data!=saved_key.data || value.data!=saved_value.data)return 1;
    if(split_setting((ps_string_view){input,strlen(input)},&key,&key)!=PS_INVALID ||
       key.data!=saved_key.data || key.size!=saved_key.size)return 1;
    if(split_setting((ps_string_view){"x=",2},&key,&value)!=PS_OK || key.size!=1 || value.size)return 1;
    puts("Published string-view example: borrowed slices, trimming and preserved error outputs passed");
    return 0;
}
