#pragma once

#ifndef _RENDER_PASS_WITH_FRAMEBUFFERS_HPP_
#define _RENDER_PASS_WITH_FRAMEBUFFERS_HPP_

#include "FrameBuffer.hpp"
#include "RenderPass.hpp"
#include <vector>

struct RenderPassWithFramebuffers {
    RenderPass renderPass;
    std::vector<Framebuffer> framebuffers;
};

#endif // _RENDER_PASS_WITH_FRAMEBUFFERS_HPP_