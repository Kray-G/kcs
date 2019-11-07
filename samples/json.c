#include <stdio.h>
#include <kcc/json.h>

int main()
{
    json_object_t *j = json_parse_file("samples/sample.json");
    if (j) {
        json_pretty_print(j, 0, 0, 0);
        json_free_all(j);
    } else {
        printf("Parse Error: %s\n", __json_status_message());
    }
    return 0;
}

