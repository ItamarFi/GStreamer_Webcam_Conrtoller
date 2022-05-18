#include "VideoStream.hpp"


VideoStream::VideoStream() {
    // init
    this->stop = FALSE;
    this->tee = gst_element_factory_make ("tee", "tee");
    this->pipeline = gst_pipeline_new ("gui_pipeline");
    this->bus = gst_element_get_bus((GstElement*) this->pipeline);

    // link
    if (!this->pipeline || !this->tee) {
        g_printerr ("%s", "Not all elements could be created.\n");
        exit(1);
    }
    gst_bin_add (GST_BIN (this->pipeline), this->tee);
}

VideoStream::~VideoStream() {
    // TODO: free resources
}

// Start the stream
void VideoStream::startPlaying() { 
    GstStateChangeReturn ret = gst_element_set_state (this->pipeline, GST_STATE_PLAYING);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("%s", "Unable to set the pipeline to the playing state.\n");
        gst_object_unref (this->pipeline);
        exit(1);
    }
    gst_bin_sync_children_states(GST_BIN (this->pipeline));
}

void VideoStream::stopPlaying() {
    GstStateChangeReturn ret = gst_element_set_state (pipeline, GST_STATE_NULL);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("%s", "Unable to set the pipeline to the null state.\n");
        gst_object_unref (pipeline);
        exit(1);
    }
    gst_bin_sync_children_states(GST_BIN (this->pipeline));
}

void VideoStream::pausePlaying() {
    GstStateChangeReturn ret = gst_element_set_state (pipeline, GST_STATE_PAUSED);
    if (ret == GST_STATE_CHANGE_FAILURE) {
        g_printerr ("%s", "Unable to set the pipeline to the pausing state.\n");
        gst_object_unref (pipeline);
        exit(1);
    }
    gst_bin_sync_children_states(GST_BIN (this->pipeline));
}


void VideoStream::addSubStream(SubVideoStream* sub, GstPadDirection direction) {

    // check if the subStream already added
    if (gst_bin_get_by_name((GstBin*)this->pipeline, sub->getName()) != NULL) {
        g_printerr("%s", "SubStream already exists.\n");
        return;
    }

    // save state to update the pipeline again (moved to pause automatically)
    GstState saved_state, saved_pending;
    gst_element_get_state((GstElement *)this->pipeline, &saved_state, &saved_pending, 0);
    // the pipeline isn't in the final state: set the pending state after the adding
    if (saved_pending != GST_STATE_VOID_PENDING) {
        saved_state = saved_pending;
    }

    // add substream to pipeline
    if (gst_bin_add(GST_BIN (this->pipeline),(GstElement *)sub->getBin()) != TRUE) {
        g_printerr("%s", "Element could not be added.\n");
        gst_object_unref (this->pipeline);
        exit(-1);
    }

    // gst_bin_sync_children_states(GST_BIN (this->pipeline));

    // create pads
    GstPad *pad_src, *pad_sink;
    if (direction == GST_PAD_SINK) {
        GstPadTemplate *templ = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(this->tee), "src_%u");
        pad_src = gst_element_request_pad (this->tee, templ, NULL, NULL);
        pad_sink = sub->getLinkPad(direction);
    }
    else if (direction == GST_PAD_SRC) {
        GstPadTemplate *templ = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(this->tee), "sink");
        pad_src = sub->getLinkPad(direction);
        pad_sink = gst_element_get_static_pad(this->tee, "sink");
        if (pad_sink == NULL) {
            g_print("%s", "\nCouldn't get sink pad from tee.\n");
            exit(-1);
        }
    }

    if ((pad_sink == NULL) || (pad_src == NULL)) {
        g_printerr("%s", "NULL pad error.\n");
        exit(-1);
    }
    
    GstPadLinkReturn ret = gst_pad_link (pad_src, pad_sink);
    if (ret != GST_PAD_LINK_OK) {
        g_printerr ("%s", "Tee could not be linked to substream.\n");
        gchar *g = g_strdup_printf("%i", (int)ret);
        g_printerr ("%s", g);
        g_printerr ("%s", "\n");
        // TODO: fix unref stuff...
        // gst_object_unref (this->pipeline);
        exit(-1);
    }
    
    if (direction == GST_PAD_SINK) {
        gst_element_link(this->tee, (GstElement *)sub->getBin());
    } else {
        gst_element_link((GstElement *)sub->getBin(), this->tee);
    }

    // GstState saved_state, saved_pending;
    // gst_element_get_state((GstElement *)this->pipeline, &saved_state, &saved_pending, 0);
    // // the pipeline isn't in the final state: set the pending state after the adding
    // if (saved_pending != GST_STATE_VOID_PENDING) {
    //     saved_state = saved_pending;
    // }

    gst_element_set_state((GstElement *)sub->getBin(), saved_state); 
    gst_element_set_state((GstElement *)this->pipeline, saved_state); 
    gst_bin_sync_children_states(GST_BIN (this->pipeline));
}

void VideoStream::linkPads(GstPad *pad_sink) {

    GstPadTemplate *templ = gst_element_class_get_pad_template(GST_ELEMENT_GET_CLASS(this->tee), "src_%u");
    GstPad *pad_src = gst_element_request_pad (this->tee, templ, NULL, NULL);

    GstPadLinkReturn ret = gst_pad_link (pad_src, pad_sink);
    if (ret != GST_PAD_LINK_OK) {
        g_printerr ("%s", "Tee could not be linked to substream.\n");
        gst_object_unref (this->pipeline);
        exit(-1);
    }
}

void VideoStream::printStatus() {
    GstState state, pending;

    // print pipeline status
    g_print("%s", "\nStates\nName\t\tState\t\tPending\n\n");

    gst_element_get_state((GstElement *)this->pipeline, &state, &pending, 0);
    g_print("%s", gst_element_get_name((GstElement *)this->pipeline));
    g_print("%s", "\t");
    g_print("%s", gst_element_state_get_name(state));
    g_print("%s", "\t\t");
    g_print("%s", gst_element_state_get_name(pending));
    g_print("%s", "\n");

    // elements: name, state, pending state
    GList *childrens = ((GstBin*)this->pipeline)->children;
    while (childrens != NULL)
    {
        gst_element_get_state((GstElement *)childrens->data, &state, &pending, 0);
        g_print("%s", gst_element_get_name((GstElement *)childrens->data));
        g_print("%s", "\t\t");
        g_print("%s", gst_element_state_get_name(state));
        g_print("%s", "\t\t");
        g_print("%s", gst_element_state_get_name(pending));
        g_print("%s", "\n");
        childrens = childrens->next;
    }
    
    // elements: source ---> sink
    g_print("%s", "\nPads\nSource\t--->\tSink\n\n");
    childrens = ((GstBin*)this->pipeline)->children;
    while (childrens != NULL)
    {
        GList *sink_pad = ((GstElement*)childrens->data)->sinkpads;
        if (sink_pad != NULL) {
            GstPad *src_pad = gst_pad_get_peer((GstPad *)sink_pad->data);
            g_print("%s", gst_element_get_name((GstElement *)gst_pad_get_parent((GstPad*)src_pad)));
            g_print("%s", "\t--->\t");
            g_print("%s", gst_element_get_name((GstElement *)gst_pad_get_parent((GstPad*)sink_pad->data)));
            g_print("%s", "\n");

        }
        childrens = childrens->next;
    }

}

GstBus* VideoStream::getBus() {
    return this->bus;
}

GstElement* VideoStream::getPipeline() {
    return this->pipeline;
}

void VideoStream::printChildrenElementsState() {
    GList *childrens = ((GstBin*)this->pipeline)->children;
    GstState state, pending;
    while (childrens != nullptr) {
        gst_element_get_state((GstElement*)childrens->data, &state, &pending, ((GstElement *)childrens->data)->start_time);
        const gchar *state_name = gst_element_state_get_name(state);
        const gchar *pending_name = gst_element_state_get_name(pending);
        const gchar *children_name = gst_element_get_name((GstElement *)childrens->data);

        // g_print("Name: %s\tState: %s\t\tPending: %s", *children_name, *state_name, *pending_name);
        g_print("Name: ");
        g_print("%s", children_name);
        g_print("\t");
        g_print("State: ");
        g_print("%s", state_name);
        g_print("\t");
        g_print("Pend: ");
        g_print("%s", pending_name);
        g_print("\n");
        childrens = childrens->next;
    }
}
