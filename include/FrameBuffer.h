#include <global.h>
#include <Texture/Texture.h>

namespace psi {

class FrameBuffer {
public:
    unsigned int fbo;
    FrameBuffer() {
        glGenFramebuffers(1, &fbo);
    }

    FrameBuffer& Use() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        return *this;
    }

    // Use() before calling this function, and EndAttach() after it
    // UseDefault() if necessary
    FrameBuffer& AttachColorBuffer(unsigned texture) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + color_attachment_count, GL_TEXTURE_2D, texture, 0);
        ++color_attachment_count;
        return *this;
    }

    FrameBuffer& EndAttach() {
        std::vector<unsigned> targets(color_attachment_count);
        for (int i = 0; i < color_attachment_count; ++i) {
            targets[i] = GL_COLOR_ATTACHMENT0 + i;
        }
        glDrawBuffers(color_attachment_count, targets.data());
        return *this;
    }

    ~FrameBuffer() {
        glDeleteFramebuffers(1, &fbo);
    }

    static void UseDefault() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
private:
    int color_attachment_count = 0;
};

}