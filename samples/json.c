#include <stdio.h>
#include <kcs/json.h>

int main()
{
    json_object_t *j = json_parse_file("samples/sample.json");
    if (j) {
        json_pretty_print(j);
        int count = json_get_element_count(j);
        printf("element count = %d\n", count);
        json_object_t *e = json_get_element(j, 1);
        json_object_t *p = json_get_property(e, "path");
        string_t *s = json_get_string(p);
        printf("json[1].path = %s\n", s ? s->cstr : "<not found..>");
        json_free_all(j);
    } else {
        printf("Parse Error: %s\n", json_error_message());
    }
    return 0;
}
