#include "scene_view.h"
#include <stdio.h>
#define CHECK(x)                                                                                   \
    do {                                                                                           \
        if (!(x)) {                                                                                \
            fprintf(stderr, "Scene view line %d\n", __LINE__);                                     \
            return 1;                                                                              \
        }                                                                                          \
    } while (0)
int main(void) {
    ps_scene scene = {0};
    for (unsigned i = 0; i < PS_MAX_OBJECTS; i++)
        ps_scene_add(&scene, PS_SPHERE, ps_v3(i, 0, 0), ps_v3(0, 0, 0), .1, 0xffffffff);
    ps_scene_view view = {0};
    ps_scene_view_sync(&view, &scene);
    view.hidden = UINT32_C(1) | (UINT32_C(1) << 31);
    CHECK(!ps_scene_view_visible(&view, 0) && !ps_scene_view_visible(&view, 31));
    CHECK(ps_scene_view_visible(&view, 1) && !ps_scene_view_visible(&view, 32));
    scene.objects[0].a.x = 42;
    ps_scene_view_sync(&view, &scene);
    CHECK(!ps_scene_view_visible(&view, 0));
    scene.objects[1].shape = PS_BOX;
    ps_scene_view_sync(&view, &scene);
    CHECK(view.hidden == 0);
    view.hidden = UINT32_MAX;
    scene.count--;
    ps_scene_view_sync(&view, &scene);
    CHECK(view.hidden == 0 && view.count == 31);
    scene.objects[0].id = 100;
    scene.objects[1].id = 200;
    ps_scene_view_sync(&view, &scene);
    view.hidden = 1;
    ps_object swap = scene.objects[0];
    scene.objects[0] = scene.objects[1];
    scene.objects[1] = swap;
    scene.count = 2;
    ps_scene_view_sync(&view, &scene);
    CHECK(ps_scene_view_visible(&view, 0) && !ps_scene_view_visible(&view, 1));
    scene.objects[1].shape = PS_POINT;
    ps_scene_view_sync(&view, &scene);
    CHECK(!ps_scene_view_visible(&view, 1));
    scene=(ps_scene){0};view=(ps_scene_view){0};
    CHECK(ps_scene_group(&scene,100,0,"Root")==PS_OK);
    CHECK(ps_scene_group(&scene,200,100,"Child")==PS_OK);
    CHECK(ps_scene_add_id(&scene,1,PS_POINT,ps_v3(0,0,0),ps_v3(0,0,0),0,UINT32_MAX)==PS_OK);
    CHECK(ps_scene_set_parent(&scene,1,200)==PS_OK);
    ps_scene_view_sync(&view,&scene);view.hidden=1;view.collapsed=2;
    CHECK(!ps_scene_view_visible(&view,0) && !ps_scene_view_visible(&view,1) && !ps_scene_view_visible(&view,2));
    CHECK(!(view.hidden&4)); /* Inherited hiding does not overwrite the child's own choice. */
    view.hidden=0;CHECK(ps_scene_view_visible(&view,2));
    view.hidden=4;
    swap=scene.objects[0];scene.objects[0]=scene.objects[2];scene.objects[2]=swap;
    ps_scene_view_sync(&view,&scene);
    CHECK(view.hidden==1 && view.collapsed==2 && !ps_scene_view_visible(&view,0) && ps_scene_view_visible(&view,2));
    view.hidden=4;
    ps_scene_view_sync(&view,&scene);
    CHECK(!ps_scene_view_visible(&view,0));
    CHECK(ps_scene_set_parent(&scene,1,0)==PS_OK);ps_scene_view_sync(&view,&scene);
    CHECK(ps_scene_view_visible(&view,0));
    puts("Scene visibility: boundary slots, motion persistence and structure reset passed");
    return 0;
}
