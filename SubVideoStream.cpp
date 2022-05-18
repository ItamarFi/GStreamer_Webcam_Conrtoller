#include "SubVideoStream.hpp"

SubVideoStream::SubVideoStream(std::vector<GstElement*> elements_vec, const gchar* name) {
    this->bin = (GstBin*)gst_bin_new(name);
    
    // add elements to bin
    for (GstElement* e : elements_vec) {
        // check elements:
        if (!gst_bin_add(this->bin, e)) {
            g_printerr ("Not all elements could be added to local bin.\n");
            exit(1);
        }
    }

    // link elements in bin
    GstElement *prev = NULL;
    for (GstElement* e : elements_vec) {
        if (prev != NULL) {
            if (!gst_element_link(prev, e)) {
                g_printerr ("Not all elements could be linked in local bin.\n");
                exit(1);
            }
        }
        prev = e;
    }
}

// TODO: free resources
SubVideoStream::~SubVideoStream() {}

GstBin *SubVideoStream::getBin() {
    return this->bin;
}

GstPad *SubVideoStream::getLinkPad(GstPadDirection direction) {
    GstPad *p = gst_bin_find_unlinked_pad(this->bin, direction);
    GstPad *ghost_p = gst_ghost_pad_new("ghostpad", p);
    gst_pad_set_active(ghost_p, TRUE);
    gst_element_add_pad((GstElement *)this->bin, ghost_p);
    gst_object_unref (p);
    return ghost_p;
}

const gchar* SubVideoStream::getName() {
    return ((GstObject*)this->bin)->name;
}
