#ifndef MRAW16
#define MRAW16

#include <godot_cpp/classes/object.hpp>
#include <godot_cpp/classes/ref.hpp>
#include <godot_cpp/classes/image.hpp>
#include <godot_cpp/classes/texture.hpp>
#include <godot_cpp/classes/image_texture.hpp>
#include <godot_cpp/classes/file_access.hpp>

using namespace godot;

class MRaw16 : public Object
{
    GDCLASS(MRaw16, Object);
private:
    /* data */

protected:
    static void _bind_methods();
public:
    MRaw16();
    ~MRaw16();
    static Ref<Image> get_texture(const String& file_path);

};







#endif