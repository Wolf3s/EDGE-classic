/*
Copyright 2018 Ioannis Makris

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
this list of conditions and the following disclaimer in the documentation and/or
other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// This file was generated by glatter.py script.



#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4702)
#endif

GLATTER_INLINE_OR_NOT
const char* enum_to_string_EGL(GLenum e)
{
    switch (e) {
#if defined(EGL_KHR_context_flush_control)
        case 0x2097: return "EGL_CONTEXT_RELEASE_BEHAVIOR_KHR";
        case 0x2098: return "EGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_KHR";
#endif
#if defined(EGL_VERSION_1_2)
        case 0x2710: return "EGL_DISPLAY_SCALING";
#endif
#if defined(EGL_VERSION_1_0)
        case 0x3000: return "EGL_SUCCESS";
        case 0x3001: return "EGL_NOT_INITIALIZED";
        case 0x3002: return "EGL_BAD_ACCESS";
        case 0x3003: return "EGL_BAD_ALLOC";
        case 0x3004: return "EGL_BAD_ATTRIBUTE";
        case 0x3005: return "EGL_BAD_CONFIG";
        case 0x3006: return "EGL_BAD_CONTEXT";
        case 0x3007: return "EGL_BAD_CURRENT_SURFACE";
        case 0x3008: return "EGL_BAD_DISPLAY";
        case 0x3009: return "EGL_BAD_MATCH";
        case 0x300a: return "EGL_BAD_NATIVE_PIXMAP";
        case 0x300b: return "EGL_BAD_NATIVE_WINDOW";
        case 0x300c: return "EGL_BAD_PARAMETER";
        case 0x300d: return "EGL_BAD_SURFACE";
#endif
#if defined(EGL_VERSION_1_1)
        case 0x300e: return "EGL_CONTEXT_LOST";
#endif
#if defined(EGL_VERSION_1_0)
        case 0x3020: return "EGL_BUFFER_SIZE";
        case 0x3021: return "EGL_ALPHA_SIZE";
        case 0x3022: return "EGL_BLUE_SIZE";
        case 0x3023: return "EGL_GREEN_SIZE";
        case 0x3024: return "EGL_RED_SIZE";
        case 0x3025: return "EGL_DEPTH_SIZE";
        case 0x3026: return "EGL_STENCIL_SIZE";
        case 0x3027: return "EGL_CONFIG_CAVEAT";
        case 0x3028: return "EGL_CONFIG_ID";
        case 0x3029: return "EGL_LEVEL";
        case 0x302a: return "EGL_MAX_PBUFFER_HEIGHT";
        case 0x302b: return "EGL_MAX_PBUFFER_PIXELS";
        case 0x302c: return "EGL_MAX_PBUFFER_WIDTH";
        case 0x302d: return "EGL_NATIVE_RENDERABLE";
        case 0x302e: return "EGL_NATIVE_VISUAL_ID";
        case 0x302f: return "EGL_NATIVE_VISUAL_TYPE";
        case 0x3031: return "EGL_SAMPLES";
        case 0x3032: return "EGL_SAMPLE_BUFFERS";
        case 0x3033: return "EGL_SURFACE_TYPE";
        case 0x3034: return "EGL_TRANSPARENT_TYPE";
        case 0x3035: return "EGL_TRANSPARENT_BLUE_VALUE";
        case 0x3036: return "EGL_TRANSPARENT_GREEN_VALUE";
        case 0x3037: return "EGL_TRANSPARENT_RED_VALUE";
        case 0x3038: return "EGL_NONE";
#endif
#if defined(EGL_VERSION_1_1)
        case 0x3039: return "EGL_BIND_TO_TEXTURE_RGB";
        case 0x303a: return "EGL_BIND_TO_TEXTURE_RGBA";
        case 0x303b: return "EGL_MIN_SWAP_INTERVAL";
        case 0x303c: return "EGL_MAX_SWAP_INTERVAL";
#endif
#if defined(EGL_VERSION_1_2)
        case 0x303d: return "EGL_LUMINANCE_SIZE";
        case 0x303e: return "EGL_ALPHA_MASK_SIZE";
        case 0x303f: return "EGL_COLOR_BUFFER_TYPE";
        case 0x3040: return "EGL_RENDERABLE_TYPE";
#endif
#if defined(EGL_VERSION_1_3)
        case 0x3041: return "EGL_MATCH_NATIVE_PIXMAP";
#endif
        case 0x3042:
#if defined(EGL_VERSION_1_3)
                    return "EGL_CONFORMANT";
#endif
#if defined(EGL_KHR_config_attribs)
                    return "EGL_CONFORMANT_KHR";
#endif
            break;
#if defined(EGL_KHR_lock_surface)
        case 0x3043: return "EGL_MATCH_FORMAT_KHR";
#endif
#if defined(EGL_VERSION_1_0)
        case 0x3050: return "EGL_SLOW_CONFIG";
        case 0x3051: return "EGL_NON_CONFORMANT_CONFIG";
        case 0x3052: return "EGL_TRANSPARENT_RGB";
        case 0x3053: return "EGL_VENDOR";
        case 0x3054: return "EGL_VERSION";
        case 0x3055: return "EGL_EXTENSIONS";
        case 0x3056: return "EGL_HEIGHT";
        case 0x3057: return "EGL_WIDTH";
        case 0x3058: return "EGL_LARGEST_PBUFFER";
        case 0x3059: return "EGL_DRAW";
        case 0x305a: return "EGL_READ";
        case 0x305b: return "EGL_CORE_NATIVE_ENGINE";
#endif
#if defined(EGL_VERSION_1_1)
        case 0x305c: return "EGL_NO_TEXTURE";
        case 0x305d: return "EGL_TEXTURE_RGB";
        case 0x305e: return "EGL_TEXTURE_RGBA";
        case 0x305f: return "EGL_TEXTURE_2D";
#endif
#if defined(EGL_NOK_texture_from_pixmap)
        case 0x307f: return "EGL_Y_INVERTED_NOK";
#endif
#if defined(EGL_VERSION_1_1)
        case 0x3080: return "EGL_TEXTURE_FORMAT";
        case 0x3081: return "EGL_TEXTURE_TARGET";
        case 0x3082: return "EGL_MIPMAP_TEXTURE";
        case 0x3083: return "EGL_MIPMAP_LEVEL";
        case 0x3084: return "EGL_BACK_BUFFER";
#endif
#if defined(EGL_VERSION_1_2)
        case 0x3085: return "EGL_SINGLE_BUFFER";
        case 0x3086: return "EGL_RENDER_BUFFER";
#endif
        case 0x3087:
#if defined(EGL_VERSION_1_2)
                    return "EGL_COLORSPACE";
#endif
#if defined(EGL_VERSION_1_3)
                    return "EGL_VG_COLORSPACE";
#endif
            break;
        case 0x3088:
#if defined(EGL_VERSION_1_2)
                    return "EGL_ALPHA_FORMAT";
#endif
#if defined(EGL_VERSION_1_3)
                    return "EGL_VG_ALPHA_FORMAT";
#endif
            break;
        case 0x3089:
#if defined(EGL_VERSION_1_2)
                    return "EGL_COLORSPACE_sRGB";
#endif
#if defined(EGL_VERSION_1_5)
                    return "EGL_GL_COLORSPACE_SRGB";
#endif
#if defined(EGL_KHR_gl_colorspace)
                    return "EGL_GL_COLORSPACE_SRGB_KHR";
#endif
#if defined(EGL_VERSION_1_3)
                    return "EGL_VG_COLORSPACE_sRGB";
#endif
            break;
        case 0x308a:
#if defined(EGL_VERSION_1_2)
                    return "EGL_COLORSPACE_LINEAR";
#endif
#if defined(EGL_VERSION_1_5)
                    return "EGL_GL_COLORSPACE_LINEAR";
#endif
#if defined(EGL_KHR_gl_colorspace)
                    return "EGL_GL_COLORSPACE_LINEAR_KHR";
#endif
#if defined(EGL_VERSION_1_3)
                    return "EGL_VG_COLORSPACE_LINEAR";
#endif
            break;
        case 0x308b:
#if defined(EGL_VERSION_1_2)
                    return "EGL_ALPHA_FORMAT_NONPRE";
#endif
#if defined(EGL_VERSION_1_3)
                    return "EGL_VG_ALPHA_FORMAT_NONPRE";
#endif
            break;
        case 0x308c:
#if defined(EGL_VERSION_1_2)
                    return "EGL_ALPHA_FORMAT_PRE";
#endif
#if defined(EGL_VERSION_1_3)
                    return "EGL_VG_ALPHA_FORMAT_PRE";
#endif
            break;
#if defined(EGL_VERSION_1_2)
        case 0x308d: return "EGL_CLIENT_APIS";
        case 0x308e: return "EGL_RGB_BUFFER";
        case 0x308f: return "EGL_LUMINANCE_BUFFER";
        case 0x3090: return "EGL_HORIZONTAL_RESOLUTION";
        case 0x3091: return "EGL_VERTICAL_RESOLUTION";
        case 0x3092: return "EGL_PIXEL_ASPECT_RATIO";
        case 0x3093: return "EGL_SWAP_BEHAVIOR";
        case 0x3094: return "EGL_BUFFER_PRESERVED";
        case 0x3095: return "EGL_BUFFER_DESTROYED";
        case 0x3096: return "EGL_OPENVG_IMAGE";
        case 0x3097: return "EGL_CONTEXT_CLIENT_TYPE";
#endif
        case 0x3098:
#if defined(EGL_VERSION_1_3)
                    return "EGL_CONTEXT_CLIENT_VERSION";
#endif
#if defined(EGL_VERSION_1_5)
                    return "EGL_CONTEXT_MAJOR_VERSION";
#endif
#if defined(EGL_KHR_create_context)
                    return "EGL_CONTEXT_MAJOR_VERSION_KHR";
#endif
            break;
#if defined(EGL_VERSION_1_4)
        case 0x3099: return "EGL_MULTISAMPLE_RESOLVE";
        case 0x309a: return "EGL_MULTISAMPLE_RESOLVE_DEFAULT";
        case 0x309b: return "EGL_MULTISAMPLE_RESOLVE_BOX";
#endif
        case 0x309c:
#if defined(EGL_VERSION_1_5)
                    return "EGL_CL_EVENT_HANDLE";
#endif
#if defined(EGL_KHR_cl_event)
                    return "EGL_CL_EVENT_HANDLE_KHR";
#endif
            break;
        case 0x309d:
#if defined(EGL_VERSION_1_5)
                    return "EGL_GL_COLORSPACE";
#endif
#if defined(EGL_KHR_gl_colorspace)
                    return "EGL_GL_COLORSPACE_KHR";
#endif
            break;
#if defined(EGL_VERSION_1_2)
        case 0x30a0: return "EGL_OPENGL_ES_API";
        case 0x30a1: return "EGL_OPENVG_API";
#endif
#if defined(EGL_VERSION_1_4)
        case 0x30a2: return "EGL_OPENGL_API";
#endif
#if defined(EGL_KHR_image)
        case 0x30b0: return "EGL_NATIVE_PIXMAP_KHR";
#endif
        case 0x30b1:
#if defined(EGL_VERSION_1_5)
                    return "EGL_GL_TEXTURE_2D";
#endif
#if defined(EGL_KHR_gl_texture_2D_image)
                    return "EGL_GL_TEXTURE_2D_KHR";
#endif
            break;
        case 0x30b2:
#if defined(EGL_VERSION_1_5)
                    return "EGL_GL_TEXTURE_3D";
#endif
#if defined(EGL_KHR_gl_texture_3D_image)
                    return "EGL_GL_TEXTURE_3D_KHR";
#endif
            break;
        case 0x30b3:
#if defined(EGL_VERSION_1_5)
                    return "EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X";
#endif
#if defined(EGL_KHR_gl_texture_cubemap_image)
                    return "EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_X_KHR";
#endif
            break;
        case 0x30b4:
#if defined(EGL_VERSION_1_5)
                    return "EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X";
#endif
#if defined(EGL_KHR_gl_texture_cubemap_image)
                    return "EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_X_KHR";
#endif
            break;
        case 0x30b5:
#if defined(EGL_VERSION_1_5)
                    return "EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y";
#endif
#if defined(EGL_KHR_gl_texture_cubemap_image)
                    return "EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Y_KHR";
#endif
            break;
        case 0x30b6:
#if defined(EGL_VERSION_1_5)
                    return "EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y";
#endif
#if defined(EGL_KHR_gl_texture_cubemap_image)
                    return "EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Y_KHR";
#endif
            break;
        case 0x30b7:
#if defined(EGL_VERSION_1_5)
                    return "EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z";
#endif
#if defined(EGL_KHR_gl_texture_cubemap_image)
                    return "EGL_GL_TEXTURE_CUBE_MAP_POSITIVE_Z_KHR";
#endif
            break;
        case 0x30b8:
#if defined(EGL_VERSION_1_5)
                    return "EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z";
#endif
#if defined(EGL_KHR_gl_texture_cubemap_image)
                    return "EGL_GL_TEXTURE_CUBE_MAP_NEGATIVE_Z_KHR";
#endif
            break;
        case 0x30b9:
#if defined(EGL_VERSION_1_5)
                    return "EGL_GL_RENDERBUFFER";
#endif
#if defined(EGL_KHR_gl_renderbuffer_image)
                    return "EGL_GL_RENDERBUFFER_KHR";
#endif
            break;
#if defined(EGL_KHR_vg_parent_image)
        case 0x30ba: return "EGL_VG_PARENT_IMAGE_KHR";
#endif
        case 0x30bc:
#if defined(EGL_VERSION_1_5)
                    return "EGL_GL_TEXTURE_LEVEL";
#endif
#if defined(EGL_KHR_gl_texture_2D_image)
                    return "EGL_GL_TEXTURE_LEVEL_KHR";
#endif
            break;
        case 0x30bd:
#if defined(EGL_VERSION_1_5)
                    return "EGL_GL_TEXTURE_ZOFFSET";
#endif
#if defined(EGL_KHR_gl_texture_3D_image)
                    return "EGL_GL_TEXTURE_ZOFFSET_KHR";
#endif
            break;
#if defined(EGL_NV_post_sub_buffer)
        case 0x30be: return "EGL_POST_SUB_BUFFER_SUPPORTED_NV";
#endif
#if defined(EGL_EXT_create_context_robustness)
        case 0x30bf: return "EGL_CONTEXT_OPENGL_ROBUST_ACCESS_EXT";
#endif
#if defined(EGL_KHR_lock_surface)
        case 0x30c0: return "EGL_FORMAT_RGB_565_EXACT_KHR";
        case 0x30c1: return "EGL_FORMAT_RGB_565_KHR";
        case 0x30c2: return "EGL_FORMAT_RGBA_8888_EXACT_KHR";
        case 0x30c3: return "EGL_FORMAT_RGBA_8888_KHR";
        case 0x30c4: return "EGL_MAP_PRESERVE_PIXELS_KHR";
        case 0x30c5: return "EGL_LOCK_USAGE_HINT_KHR";
        case 0x30c6: return "EGL_BITMAP_POINTER_KHR";
        case 0x30c7: return "EGL_BITMAP_PITCH_KHR";
        case 0x30c8: return "EGL_BITMAP_ORIGIN_KHR";
        case 0x30c9: return "EGL_BITMAP_PIXEL_RED_OFFSET_KHR";
        case 0x30ca: return "EGL_BITMAP_PIXEL_GREEN_OFFSET_KHR";
        case 0x30cb: return "EGL_BITMAP_PIXEL_BLUE_OFFSET_KHR";
        case 0x30cc: return "EGL_BITMAP_PIXEL_ALPHA_OFFSET_KHR";
        case 0x30cd: return "EGL_BITMAP_PIXEL_LUMINANCE_OFFSET_KHR";
        case 0x30ce: return "EGL_LOWER_LEFT_KHR";
        case 0x30cf: return "EGL_UPPER_LEFT_KHR";
#endif
        case 0x30d2:
#if defined(EGL_VERSION_1_5)
                    return "EGL_IMAGE_PRESERVED";
#endif
#if defined(EGL_KHR_image_base)
                    return "EGL_IMAGE_PRESERVED_KHR";
#endif
            break;
#if defined(EGL_NV_coverage_sample)
        case 0x30e0: return "EGL_COVERAGE_BUFFERS_NV";
        case 0x30e1: return "EGL_COVERAGE_SAMPLES_NV";
#endif
#if defined(EGL_NV_depth_nonlinear)
        case 0x30e2: return "EGL_DEPTH_ENCODING_NV";
        case 0x30e3: return "EGL_DEPTH_ENCODING_NONLINEAR_NV";
#endif
#if defined(KHRONOS_SUPPORT_INT64)
        case 0x30e6: return "EGL_SYNC_PRIOR_COMMANDS_COMPLETE_NV";
        case 0x30e7: return "EGL_SYNC_STATUS_NV";
        case 0x30e8: return "EGL_SIGNALED_NV";
        case 0x30e9: return "EGL_UNSIGNALED_NV";
        case 0x30ea: return "EGL_ALREADY_SIGNALED_NV";
        case 0x30eb: return "EGL_TIMEOUT_EXPIRED_NV";
        case 0x30ec: return "EGL_CONDITION_SATISFIED_NV";
        case 0x30ed: return "EGL_SYNC_TYPE_NV";
        case 0x30ee: return "EGL_SYNC_CONDITION_NV";
        case 0x30ef: return "EGL_SYNC_FENCE_NV";
#endif
        case 0x30f0:
#if defined(EGL_VERSION_1_5)
                    return "EGL_SYNC_PRIOR_COMMANDS_COMPLETE";
#endif
#if defined(KHRONOS_SUPPORT_INT64)
                    return "EGL_SYNC_PRIOR_COMMANDS_COMPLETE_KHR";
#endif
            break;
        case 0x30f1:
#if defined(EGL_VERSION_1_5)
                    return "EGL_SYNC_STATUS";
#endif
#if defined(KHRONOS_SUPPORT_INT64)
                    return "EGL_SYNC_STATUS_KHR";
#endif
            break;
        case 0x30f2:
#if defined(EGL_VERSION_1_5)
                    return "EGL_SIGNALED";
#endif
#if defined(KHRONOS_SUPPORT_INT64)
                    return "EGL_SIGNALED_KHR";
#endif
            break;
        case 0x30f3:
#if defined(EGL_VERSION_1_5)
                    return "EGL_UNSIGNALED";
#endif
#if defined(KHRONOS_SUPPORT_INT64)
                    return "EGL_UNSIGNALED_KHR";
#endif
            break;
        case 0x30f5:
#if defined(EGL_VERSION_1_5)
                    return "EGL_TIMEOUT_EXPIRED";
#endif
#if defined(KHRONOS_SUPPORT_INT64)
                    return "EGL_TIMEOUT_EXPIRED_KHR";
#endif
            break;
        case 0x30f6:
#if defined(EGL_VERSION_1_5)
                    return "EGL_CONDITION_SATISFIED";
#endif
#if defined(KHRONOS_SUPPORT_INT64)
                    return "EGL_CONDITION_SATISFIED_KHR";
#endif
            break;
        case 0x30f7:
#if defined(EGL_VERSION_1_5)
                    return "EGL_SYNC_TYPE";
#endif
#if defined(KHRONOS_SUPPORT_INT64)
                    return "EGL_SYNC_TYPE_KHR";
#endif
            break;
        case 0x30f8:
#if defined(EGL_VERSION_1_5)
                    return "EGL_SYNC_CONDITION";
#endif
#if defined(KHRONOS_SUPPORT_INT64)
                    return "EGL_SYNC_CONDITION_KHR";
#endif
            break;
        case 0x30f9:
#if defined(EGL_VERSION_1_5)
                    return "EGL_SYNC_FENCE";
#endif
#if defined(KHRONOS_SUPPORT_INT64)
                    return "EGL_SYNC_FENCE_KHR";
#endif
            break;
#if defined(KHRONOS_SUPPORT_INT64)
        case 0x30fa: return "EGL_SYNC_REUSABLE_KHR";
#endif
        case 0x30fb:
#if defined(EGL_VERSION_1_5)
                    return "EGL_CONTEXT_MINOR_VERSION";
#endif
#if defined(EGL_KHR_create_context)
                    return "EGL_CONTEXT_MINOR_VERSION_KHR";
#endif
            break;
#if defined(EGL_KHR_create_context)
        case 0x30fc: return "EGL_CONTEXT_FLAGS_KHR";
#endif
        case 0x30fd:
#if defined(EGL_VERSION_1_5)
                    return "EGL_CONTEXT_OPENGL_PROFILE_MASK";
#endif
#if defined(EGL_KHR_create_context)
                    return "EGL_CONTEXT_OPENGL_PROFILE_MASK_KHR";
#endif
            break;
        case 0x30fe:
#if defined(EGL_VERSION_1_5)
                    return "EGL_SYNC_CL_EVENT";
#endif
#if defined(EGL_KHR_cl_event)
                    return "EGL_SYNC_CL_EVENT_KHR";
#endif
            break;
        case 0x30ff:
#if defined(EGL_VERSION_1_5)
                    return "EGL_SYNC_CL_EVENT_COMPLETE";
#endif
#if defined(EGL_KHR_cl_event)
                    return "EGL_SYNC_CL_EVENT_COMPLETE_KHR";
#endif
            break;
#if defined(EGL_IMG_context_priority)
        case 0x3100: return "EGL_CONTEXT_PRIORITY_LEVEL_IMG";
        case 0x3101: return "EGL_CONTEXT_PRIORITY_HIGH_IMG";
        case 0x3102: return "EGL_CONTEXT_PRIORITY_MEDIUM_IMG";
        case 0x3103: return "EGL_CONTEXT_PRIORITY_LOW_IMG";
#endif
#if defined(EGL_IMG_image_plane_attribs)
        case 0x3105: return "EGL_NATIVE_BUFFER_MULTIPLANE_SEPARATE_IMG";
        case 0x3106: return "EGL_NATIVE_BUFFER_PLANE_OFFSET_IMG";
#endif
#if defined(EGL_KHR_lock_surface2)
        case 0x3110: return "EGL_BITMAP_PIXEL_SIZE_KHR";
#endif
#if defined(EGL_NV_coverage_sample_resolve)
        case 0x3131: return "EGL_COVERAGE_SAMPLE_RESOLVE_NV";
        case 0x3132: return "EGL_COVERAGE_SAMPLE_RESOLVE_DEFAULT_NV";
        case 0x3133: return "EGL_COVERAGE_SAMPLE_RESOLVE_NONE_NV";
#endif
#if defined(EGL_EXT_multiview_window)
        case 0x3134: return "EGL_MULTIVIEW_VIEW_COUNT_EXT";
#endif
#if defined(EGL_NV_3dvision_surface)
        case 0x3136: return "EGL_AUTO_STEREO_NV";
#endif
#if defined(EGL_EXT_create_context_robustness)
        case 0x3138: return "EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_EXT";
#endif
        case 0x313d:
#if defined(EGL_EXT_buffer_age)
                    return "EGL_BUFFER_AGE_EXT";
#endif
#if defined(EGL_KHR_partial_update)
                    return "EGL_BUFFER_AGE_KHR";
#endif
            break;
#if defined(EGL_EXT_platform_device)
        case 0x313f: return "EGL_PLATFORM_DEVICE_EXT";
#endif
#if defined(EGL_ANDROID_image_native_buffer)
        case 0x3140: return "EGL_NATIVE_BUFFER_ANDROID";
#endif
#if defined(EGL_KHR_platform_android)
        case 0x3141: return "EGL_PLATFORM_ANDROID_KHR";
#endif
#if defined(EGL_ANDROID_recordable)
        case 0x3142: return "EGL_RECORDABLE_ANDROID";
#endif
#if defined(EGL_ANDROID_create_native_client_buffer)
        case 0x3143: return "EGL_NATIVE_BUFFER_USAGE_ANDROID";
#endif
#if defined(EGL_ANDROID_native_fence_sync)
        case 0x3144: return "EGL_SYNC_NATIVE_FENCE_ANDROID";
        case 0x3145: return "EGL_SYNC_NATIVE_FENCE_FD_ANDROID";
        case 0x3146: return "EGL_SYNC_NATIVE_FENCE_SIGNALED_ANDROID";
#endif
#if defined(EGL_ANDROID_framebuffer_target)
        case 0x3147: return "EGL_FRAMEBUFFER_TARGET_ANDROID";
#endif
#if defined(EGL_ANDROID_front_buffer_auto_refresh)
        case 0x314c: return "EGL_FRONT_BUFFER_AUTO_REFRESH_ANDROID";
#endif
#if defined(EGL_VERSION_1_5)
        case 0x31b0: return "EGL_CONTEXT_OPENGL_DEBUG";
        case 0x31b1: return "EGL_CONTEXT_OPENGL_FORWARD_COMPATIBLE";
        case 0x31b2: return "EGL_CONTEXT_OPENGL_ROBUST_ACCESS";
#endif
#if defined(EGL_KHR_create_context_no_error)
        case 0x31b3: return "EGL_CONTEXT_OPENGL_NO_ERROR_KHR";
#endif
        case 0x31bd:
#if defined(EGL_VERSION_1_5)
                    return "EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY";
#endif
#if defined(EGL_KHR_create_context)
                    return "EGL_CONTEXT_OPENGL_RESET_NOTIFICATION_STRATEGY_KHR";
#endif
            break;
        case 0x31be:
#if defined(EGL_VERSION_1_5)
                    return "EGL_NO_RESET_NOTIFICATION";
#endif
#if defined(EGL_EXT_create_context_robustness)
                    return "EGL_NO_RESET_NOTIFICATION_EXT";
#endif
#if defined(EGL_KHR_create_context)
                    return "EGL_NO_RESET_NOTIFICATION_KHR";
#endif
            break;
        case 0x31bf:
#if defined(EGL_VERSION_1_5)
                    return "EGL_LOSE_CONTEXT_ON_RESET";
#endif
#if defined(EGL_EXT_create_context_robustness)
                    return "EGL_LOSE_CONTEXT_ON_RESET_EXT";
#endif
#if defined(EGL_KHR_create_context)
                    return "EGL_LOSE_CONTEXT_ON_RESET_KHR";
#endif
            break;
#if defined(EGL_MESA_drm_image)
        case 0x31d0: return "EGL_DRM_BUFFER_FORMAT_MESA";
        case 0x31d1: return "EGL_DRM_BUFFER_USE_MESA";
        case 0x31d2: return "EGL_DRM_BUFFER_FORMAT_ARGB32_MESA";
        case 0x31d3: return "EGL_DRM_BUFFER_MESA";
        case 0x31d4: return "EGL_DRM_BUFFER_STRIDE_MESA";
#endif
        case 0x31d5:
#if defined(EGL_EXT_platform_x11)
                    return "EGL_PLATFORM_X11_EXT";
#endif
#if defined(EGL_KHR_platform_x11)
                    return "EGL_PLATFORM_X11_KHR";
#endif
            break;
        case 0x31d6:
#if defined(EGL_EXT_platform_x11)
                    return "EGL_PLATFORM_X11_SCREEN_EXT";
#endif
#if defined(EGL_KHR_platform_x11)
                    return "EGL_PLATFORM_X11_SCREEN_KHR";
#endif
            break;
        case 0x31d7:
#if defined(EGL_KHR_platform_gbm)
                    return "EGL_PLATFORM_GBM_KHR";
#endif
#if defined(EGL_MESA_platform_gbm)
                    return "EGL_PLATFORM_GBM_MESA";
#endif
            break;
        case 0x31d8:
#if defined(EGL_EXT_platform_wayland)
                    return "EGL_PLATFORM_WAYLAND_EXT";
#endif
#if defined(EGL_KHR_platform_wayland)
                    return "EGL_PLATFORM_WAYLAND_KHR";
#endif
            break;
#if defined(EGL_MESA_platform_surfaceless)
        case 0x31dd: return "EGL_PLATFORM_SURFACELESS_MESA";
#endif
#if defined(EGL_KHR_stream)
        case 0x31fc: return "EGL_STREAM_FIFO_LENGTH_KHR";
        case 0x31fd: return "EGL_STREAM_TIME_NOW_KHR";
        case 0x31fe: return "EGL_STREAM_TIME_CONSUMER_KHR";
        case 0x31ff: return "EGL_STREAM_TIME_PRODUCER_KHR";
#endif
#if defined(EGL_ANGLE_d3d_share_handle_client_buffer)
        case 0x3200: return "EGL_D3D_TEXTURE_2D_SHARE_HANDLE_ANGLE";
#endif
#if defined(EGL_ANGLE_window_fixed_size)
        case 0x3201: return "EGL_FIXED_SIZE_ANGLE";
#endif
#if defined(KHRONOS_SUPPORT_INT64)
        case 0x3210: return "EGL_CONSUMER_LATENCY_USEC_KHR";
        case 0x3212: return "EGL_PRODUCER_FRAME_KHR";
        case 0x3213: return "EGL_CONSUMER_FRAME_KHR";
        case 0x3214: return "EGL_STREAM_STATE_KHR";
        case 0x3215: return "EGL_STREAM_STATE_CREATED_KHR";
        case 0x3216: return "EGL_STREAM_STATE_CONNECTING_KHR";
        case 0x3217: return "EGL_STREAM_STATE_EMPTY_KHR";
        case 0x3218: return "EGL_STREAM_STATE_NEW_FRAME_AVAILABLE_KHR";
        case 0x3219: return "EGL_STREAM_STATE_OLD_FRAME_AVAILABLE_KHR";
        case 0x321a: return "EGL_STREAM_STATE_DISCONNECTED_KHR";
        case 0x321b: return "EGL_BAD_STREAM_KHR";
        case 0x321c: return "EGL_BAD_STATE_KHR";
#endif
#if defined(EGL_KHR_stream)
        case 0x321e: return "EGL_CONSUMER_ACQUIRE_TIMEOUT_USEC_KHR";
#endif
#if defined(EGL_NV_stream_sync)
        case 0x321f: return "EGL_SYNC_NEW_FRAME_NV";
#endif
#if defined(EGL_EXT_device_base)
        case 0x322b: return "EGL_BAD_DEVICE_EXT";
        case 0x322c: return "EGL_DEVICE_EXT";
#endif
#if defined(EGL_EXT_output_base)
        case 0x322d: return "EGL_BAD_OUTPUT_LAYER_EXT";
        case 0x322e: return "EGL_BAD_OUTPUT_PORT_EXT";
        case 0x322f: return "EGL_SWAP_INTERVAL_EXT";
#endif
#if defined(EGL_EXT_device_drm)
        case 0x3233: return "EGL_DRM_DEVICE_FILE_EXT";
#endif
#if defined(EGL_EXT_output_drm)
        case 0x3234: return "EGL_DRM_CRTC_EXT";
        case 0x3235: return "EGL_DRM_PLANE_EXT";
        case 0x3236: return "EGL_DRM_CONNECTOR_EXT";
#endif
#if defined(EGL_EXT_device_openwf)
        case 0x3237: return "EGL_OPENWF_DEVICE_ID_EXT";
#endif
#if defined(EGL_EXT_output_openwf)
        case 0x3238: return "EGL_OPENWF_PIPELINE_ID_EXT";
        case 0x3239: return "EGL_OPENWF_PORT_ID_EXT";
#endif
#if defined(EGL_NV_device_cuda)
        case 0x323a: return "EGL_CUDA_DEVICE_NV";
#endif
#if defined(EGL_NV_cuda_event)
        case 0x323b: return "EGL_CUDA_EVENT_HANDLE_NV";
        case 0x323c: return "EGL_SYNC_CUDA_EVENT_NV";
        case 0x323d: return "EGL_SYNC_CUDA_EVENT_COMPLETE_NV";
#endif
#if defined(EGL_NV_stream_cross_partition)
        case 0x323f: return "EGL_STREAM_CROSS_PARTITION_NV";
#endif
#if defined(EGL_NV_stream_remote)
        case 0x3240: return "EGL_STREAM_STATE_INITIALIZING_NV";
        case 0x3241: return "EGL_STREAM_TYPE_NV";
        case 0x3242: return "EGL_STREAM_PROTOCOL_NV";
        case 0x3243: return "EGL_STREAM_ENDPOINT_NV";
        case 0x3244: return "EGL_STREAM_LOCAL_NV";
#endif
#if defined(EGL_NV_stream_cross_process)
        case 0x3245: return "EGL_STREAM_CROSS_PROCESS_NV";
#endif
#if defined(EGL_NV_stream_remote)
        case 0x3246: return "EGL_STREAM_PROTOCOL_FD_NV";
        case 0x3247: return "EGL_STREAM_PRODUCER_NV";
        case 0x3248: return "EGL_STREAM_CONSUMER_NV";
#endif
#if defined(EGL_NV_stream_socket)
        case 0x324b: return "EGL_STREAM_PROTOCOL_SOCKET_NV";
        case 0x324c: return "EGL_SOCKET_HANDLE_NV";
        case 0x324d: return "EGL_SOCKET_TYPE_NV";
#endif
#if defined(EGL_NV_stream_socket_unix)
        case 0x324e: return "EGL_SOCKET_TYPE_UNIX_NV";
#endif
#if defined(EGL_NV_stream_socket_inet)
        case 0x324f: return "EGL_SOCKET_TYPE_INET_NV";
#endif
#if defined(EGL_NV_stream_metadata)
        case 0x3250: return "EGL_MAX_STREAM_METADATA_BLOCKS_NV";
        case 0x3251: return "EGL_MAX_STREAM_METADATA_BLOCK_SIZE_NV";
        case 0x3252: return "EGL_MAX_STREAM_METADATA_TOTAL_SIZE_NV";
        case 0x3253: return "EGL_PRODUCER_METADATA_NV";
        case 0x3254: return "EGL_CONSUMER_METADATA_NV";
        case 0x3255: return "EGL_METADATA0_SIZE_NV";
        case 0x3256: return "EGL_METADATA1_SIZE_NV";
        case 0x3257: return "EGL_METADATA2_SIZE_NV";
        case 0x3258: return "EGL_METADATA3_SIZE_NV";
        case 0x3259: return "EGL_METADATA0_TYPE_NV";
        case 0x325a: return "EGL_METADATA1_TYPE_NV";
        case 0x325b: return "EGL_METADATA2_TYPE_NV";
        case 0x325c: return "EGL_METADATA3_TYPE_NV";
#endif
#if defined(EGL_EXT_image_dma_buf_import)
        case 0x3270: return "EGL_LINUX_DMA_BUF_EXT";
        case 0x3271: return "EGL_LINUX_DRM_FOURCC_EXT";
        case 0x3272: return "EGL_DMA_BUF_PLANE0_FD_EXT";
        case 0x3273: return "EGL_DMA_BUF_PLANE0_OFFSET_EXT";
        case 0x3274: return "EGL_DMA_BUF_PLANE0_PITCH_EXT";
        case 0x3275: return "EGL_DMA_BUF_PLANE1_FD_EXT";
        case 0x3276: return "EGL_DMA_BUF_PLANE1_OFFSET_EXT";
        case 0x3277: return "EGL_DMA_BUF_PLANE1_PITCH_EXT";
        case 0x3278: return "EGL_DMA_BUF_PLANE2_FD_EXT";
        case 0x3279: return "EGL_DMA_BUF_PLANE2_OFFSET_EXT";
        case 0x327a: return "EGL_DMA_BUF_PLANE2_PITCH_EXT";
        case 0x327b: return "EGL_YUV_COLOR_SPACE_HINT_EXT";
        case 0x327c: return "EGL_SAMPLE_RANGE_HINT_EXT";
        case 0x327d: return "EGL_YUV_CHROMA_HORIZONTAL_SITING_HINT_EXT";
        case 0x327e: return "EGL_YUV_CHROMA_VERTICAL_SITING_HINT_EXT";
        case 0x327f: return "EGL_ITU_REC601_EXT";
        case 0x3280: return "EGL_ITU_REC709_EXT";
        case 0x3281: return "EGL_ITU_REC2020_EXT";
        case 0x3282: return "EGL_YUV_FULL_RANGE_EXT";
        case 0x3283: return "EGL_YUV_NARROW_RANGE_EXT";
        case 0x3284: return "EGL_YUV_CHROMA_SITING_0_EXT";
        case 0x3285: return "EGL_YUV_CHROMA_SITING_0_5_EXT";
#endif
#if defined(EGL_ARM_pixmap_multisample_discard)
        case 0x3286: return "EGL_DISCARD_SAMPLES_ARM";
#endif
#if defined(EGL_ARM_implicit_external_sync)
        case 0x328a: return "EGL_SYNC_PRIOR_COMMANDS_IMPLICIT_EXTERNAL_ARM";
#endif
#if defined(EGL_TIZEN_image_native_buffer)
        case 0x32a0: return "EGL_NATIVE_BUFFER_TIZEN";
#endif
#if defined(EGL_TIZEN_image_native_surface)
        case 0x32a1: return "EGL_NATIVE_SURFACE_TIZEN";
#endif
#if defined(EGL_EXT_protected_content)
        case 0x32c0: return "EGL_PROTECTED_CONTENT_EXT";
#endif
#if defined(EGL_EXT_yuv_surface)
        case 0x3300: return "EGL_YUV_BUFFER_EXT";
        case 0x3301: return "EGL_YUV_ORDER_EXT";
        case 0x3302: return "EGL_YUV_ORDER_YUV_EXT";
        case 0x3303: return "EGL_YUV_ORDER_YVU_EXT";
        case 0x3304: return "EGL_YUV_ORDER_YUYV_EXT";
        case 0x3305: return "EGL_YUV_ORDER_UYVY_EXT";
        case 0x3306: return "EGL_YUV_ORDER_YVYU_EXT";
        case 0x3307: return "EGL_YUV_ORDER_VYUY_EXT";
        case 0x3308: return "EGL_YUV_ORDER_AYUV_EXT";
        case 0x330a: return "EGL_YUV_CSC_STANDARD_EXT";
        case 0x330b: return "EGL_YUV_CSC_STANDARD_601_EXT";
        case 0x330c: return "EGL_YUV_CSC_STANDARD_709_EXT";
        case 0x330d: return "EGL_YUV_CSC_STANDARD_2020_EXT";
        case 0x3311: return "EGL_YUV_NUMBER_OF_PLANES_EXT";
        case 0x3312: return "EGL_YUV_SUBSAMPLE_EXT";
        case 0x3313: return "EGL_YUV_SUBSAMPLE_4_2_0_EXT";
        case 0x3314: return "EGL_YUV_SUBSAMPLE_4_2_2_EXT";
        case 0x3315: return "EGL_YUV_SUBSAMPLE_4_4_4_EXT";
        case 0x3317: return "EGL_YUV_DEPTH_RANGE_EXT";
        case 0x3318: return "EGL_YUV_DEPTH_RANGE_LIMITED_EXT";
        case 0x3319: return "EGL_YUV_DEPTH_RANGE_FULL_EXT";
        case 0x331a: return "EGL_YUV_PLANE_BPP_EXT";
        case 0x331b: return "EGL_YUV_PLANE_BPP_0_EXT";
        case 0x331c: return "EGL_YUV_PLANE_BPP_8_EXT";
        case 0x331d: return "EGL_YUV_PLANE_BPP_10_EXT";
#endif
#if defined(EGL_NV_stream_metadata)
        case 0x3328: return "EGL_PENDING_METADATA_NV";
#endif
#if defined(EGL_NV_stream_fifo_next)
        case 0x3329: return "EGL_PENDING_FRAME_NV";
        case 0x332a: return "EGL_STREAM_TIME_PENDING_NV";
#endif
#if defined(EGL_NV_stream_consumer_gltexture_yuv)
        case 0x332c: return "EGL_YUV_PLANE0_TEXTURE_UNIT_NV";
        case 0x332d: return "EGL_YUV_PLANE1_TEXTURE_UNIT_NV";
        case 0x332e: return "EGL_YUV_PLANE2_TEXTURE_UNIT_NV";
#endif
#if defined(EGL_NV_stream_reset)
        case 0x3334: return "EGL_SUPPORT_RESET_NV";
        case 0x3335: return "EGL_SUPPORT_REUSE_NV";
#endif
#if defined(EGL_NV_stream_fifo_synchronous)
        case 0x3336: return "EGL_STREAM_FIFO_SYNCHRONOUS_NV";
#endif
#if defined(EGL_NV_stream_frame_limits)
        case 0x3337: return "EGL_PRODUCER_MAX_FRAME_HINT_NV";
        case 0x3338: return "EGL_CONSUMER_MAX_FRAME_HINT_NV";
#endif
#if defined(EGL_EXT_pixel_format_float)
        case 0x3339: return "EGL_COLOR_COMPONENT_TYPE_EXT";
        case 0x333a: return "EGL_COLOR_COMPONENT_TYPE_FIXED_EXT";
        case 0x333b: return "EGL_COLOR_COMPONENT_TYPE_FLOAT_EXT";
#endif
#if defined(EGL_EXT_gl_colorspace_bt2020_linear)
        case 0x333f: return "EGL_GL_COLORSPACE_BT2020_LINEAR_EXT";
#endif
#if defined(EGL_EXT_gl_colorspace_bt2020_pq)
        case 0x3340: return "EGL_GL_COLORSPACE_BT2020_PQ_EXT";
#endif
#if defined(EGL_EXT_surface_SMPTE2086_metadata)
        case 0x3341: return "EGL_SMPTE2086_DISPLAY_PRIMARY_RX_EXT";
        case 0x3342: return "EGL_SMPTE2086_DISPLAY_PRIMARY_RY_EXT";
        case 0x3343: return "EGL_SMPTE2086_DISPLAY_PRIMARY_GX_EXT";
        case 0x3344: return "EGL_SMPTE2086_DISPLAY_PRIMARY_GY_EXT";
        case 0x3345: return "EGL_SMPTE2086_DISPLAY_PRIMARY_BX_EXT";
        case 0x3346: return "EGL_SMPTE2086_DISPLAY_PRIMARY_BY_EXT";
        case 0x3347: return "EGL_SMPTE2086_WHITE_POINT_X_EXT";
        case 0x3348: return "EGL_SMPTE2086_WHITE_POINT_Y_EXT";
        case 0x3349: return "EGL_SMPTE2086_MAX_LUMINANCE_EXT";
        case 0x334a: return "EGL_SMPTE2086_MIN_LUMINANCE_EXT";
#endif
#if defined(EGL_NV_robustness_video_memory_purge)
        case 0x334c: return "EGL_GENERATE_RESET_ON_VIDEO_MEMORY_PURGE_NV";
#endif
#if defined(EGL_NV_stream_cross_object)
        case 0x334d: return "EGL_STREAM_CROSS_OBJECT_NV";
#endif
#if defined(EGL_NV_stream_cross_display)
        case 0x334e: return "EGL_STREAM_CROSS_DISPLAY_NV";
#endif
#if defined(EGL_NV_stream_cross_system)
        case 0x334f: return "EGL_STREAM_CROSS_SYSTEM_NV";
#endif
#if defined(EGL_EXT_gl_colorspace_scrgb_linear)
        case 0x3350: return "EGL_GL_COLORSPACE_SCRGB_LINEAR_EXT";
#endif
#if defined(EGL_ANGLE_device_d3d)
        case 0x33a0: return "EGL_D3D9_DEVICE_ANGLE";
        case 0x33a1: return "EGL_D3D11_DEVICE_ANGLE";
#endif
#if defined(EGL_KHR_debug)
        case 0x33b0: return "EGL_OBJECT_THREAD_KHR";
        case 0x33b1: return "EGL_OBJECT_DISPLAY_KHR";
        case 0x33b2: return "EGL_OBJECT_CONTEXT_KHR";
        case 0x33b3: return "EGL_OBJECT_SURFACE_KHR";
        case 0x33b4: return "EGL_OBJECT_IMAGE_KHR";
        case 0x33b5: return "EGL_OBJECT_SYNC_KHR";
        case 0x33b6: return "EGL_OBJECT_STREAM_KHR";
        case 0x33b8: return "EGL_DEBUG_CALLBACK_KHR";
        case 0x33b9: return "EGL_DEBUG_MSG_CRITICAL_KHR";
        case 0x33ba: return "EGL_DEBUG_MSG_ERROR_KHR";
        case 0x33bb: return "EGL_DEBUG_MSG_WARN_KHR";
        case 0x33bc: return "EGL_DEBUG_MSG_INFO_KHR";
#endif
#if defined(EGL_EXT_image_dma_buf_import_modifiers)
        case 0x3440: return "EGL_DMA_BUF_PLANE3_FD_EXT";
        case 0x3441: return "EGL_DMA_BUF_PLANE3_OFFSET_EXT";
        case 0x3442: return "EGL_DMA_BUF_PLANE3_PITCH_EXT";
        case 0x3443: return "EGL_DMA_BUF_PLANE0_MODIFIER_LO_EXT";
        case 0x3444: return "EGL_DMA_BUF_PLANE0_MODIFIER_HI_EXT";
        case 0x3445: return "EGL_DMA_BUF_PLANE1_MODIFIER_LO_EXT";
        case 0x3446: return "EGL_DMA_BUF_PLANE1_MODIFIER_HI_EXT";
        case 0x3447: return "EGL_DMA_BUF_PLANE2_MODIFIER_LO_EXT";
        case 0x3448: return "EGL_DMA_BUF_PLANE2_MODIFIER_HI_EXT";
        case 0x3449: return "EGL_DMA_BUF_PLANE3_MODIFIER_LO_EXT";
        case 0x344a: return "EGL_DMA_BUF_PLANE3_MODIFIER_HI_EXT";
#endif
#if defined(EGL_EXT_compositor)
        case 0x3460: return "EGL_PRIMARY_COMPOSITOR_CONTEXT_EXT";
        case 0x3461: return "EGL_EXTERNAL_REF_ID_EXT";
        case 0x3462: return "EGL_COMPOSITOR_DROP_NEWEST_FRAME_EXT";
        case 0x3463: return "EGL_COMPOSITOR_KEEP_NEWEST_FRAME_EXT";
#endif
#if defined(EGL_HI_colorformats)
        case 0x8f70: return "EGL_COLOR_FORMAT_HI";
        case 0x8f71: return "EGL_COLOR_RGB_HI";
        case 0x8f72: return "EGL_COLOR_RGBA_HI";
        case 0x8f73: return "EGL_COLOR_ARGB_HI";
#endif
#if defined(EGL_HI_clientpixmap)
        case 0x8f74: return "EGL_CLIENT_PIXMAP_POINTER_HI";
#endif
    }
    return "<UNKNOWN ENUM>";
}

#ifdef _MSC_VER
#pragma warning(pop) 
#endif

