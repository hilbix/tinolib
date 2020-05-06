
{{MAP files /}}
{{GLOB lib/*.h}}
{{.}}

{{LOOP files}}
#include "{{2}}"
{{.}}

