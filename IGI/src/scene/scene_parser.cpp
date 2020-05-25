#include "igiscene/scene_parser.h"
#include "rapidjson/stringbuffer.h"
#include "rapidjson/writer.h"
#include "rflite/rflite.h"

//  json strcuture:
//
//  {
//      *background: { r, g, b },
//      material: [
//          {
//              type,
//              name,
//              params, ...
//          }, ...
//      ],
//      surface: [
//          {
//              type,
//              name,
//              params, ...
//          }, ...
//      ],
//      entity: [
//          {
//              material,
//              surface,
//              position,
//              *rotation,
//              *scale,
//          }, ...
//      ],
//      camera: {
//          position,
//          rotation,
//          *type,
//          params, ...
//      }
//      integrator: {
//          *type,
//          params, ...
//      },
//      spp: number
//  }

using namespace igi;
using namespace rapidjson;
using namespace rflite;

const char *const entry_background = "background";
const char *const entry_material   = "material";

color3 parse_color3(Document &doc) {
    if (!doc.HasMember(entry_background))
        return palette::black;

    const Value &val = doc[entry_background];
    assert(val.IsArray());
    assert(val.Size() < 4);

    color3 res = palette::black;
    for (SizeType i = 0; i < 3; i++) {
        const Value &c = val[i];
        assert(c.IsDouble);
        res[i] = static_cast<col_c_t>(c.GetDouble());
    }

    return res;
}

scene_parser::parse_res_t scene_parser::Parse(Document &doc, const allocator_type &alloc) {
    assert(doc.IsObject());

    color3 back = parse_color3(doc);

    std::pmr::vector<IMaterial *> mats(alloc);
    const Value &m = doc[entry_material];
}