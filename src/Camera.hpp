#pragma once

#include <algorithm>
#include <cmath>
#include <optional>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class Camera {
public:
    Camera(glm::vec2 initialPosition = glm::vec2(0.0f), float initialZoom = 1.0f)
        : position(initialPosition), zoom(std::isfinite(initialZoom)
            ? std::clamp(initialZoom, minZoom, maxZoom) : 1.0f) {}

    glm::mat4 GetViewMatrix(float aspectRatio) const {
        const glm::mat4 projection = glm::ortho(-aspectRatio, aspectRatio, -1.0f, 1.0f, -1.0f, 1.0f);
        glm::mat4 view = glm::scale(glm::mat4(1.0f), glm::vec3(zoom, zoom, 1.0f));
        view = glm::translate(view, glm::vec3(position.x, position.y, 0.0f));
        return projection * view;
    }

    glm::dvec2 NdcToWorld(glm::dvec2 ndc, double aspectRatio) const {
        return NdcToViewOffset(ndc, aspectRatio) - glm::dvec2(position);
    }

    void ProcessMouseScroll(double yoffset) {
        if (!std::isfinite(yoffset) || yoffset == 0.0) return;

        // Clamp in log space before exponentiating, even for very large scroll offsets.
        const double logZoom = std::log(static_cast<double>(zoom)) + yoffset * std::log(1.1);
        zoom = static_cast<float>(std::exp(std::clamp(logZoom,
            std::log(static_cast<double>(minZoom)), std::log(static_cast<double>(maxZoom)))));
    }

    void ProcessMouseMovement(glm::dvec2 cursor, glm::dvec2 windowSize, double aspectRatio) {
        if (lastCursor) {
            const glm::dvec2 delta = cursor - *lastCursor;
            const glm::dvec2 ndcDelta(2.0 * delta.x / windowSize.x, -2.0 * delta.y / windowSize.y);
            position += glm::vec2(NdcToViewOffset(ndcDelta, aspectRatio));
        }
        lastCursor = cursor;
    }

    void StopPanning() {
        lastCursor.reset();
    }

private:
    static constexpr float minZoom = 0.01f;
    static constexpr float maxZoom = 2.0f;

    glm::vec2 position;
    float zoom;
    std::optional<glm::dvec2> lastCursor;

    glm::dvec2 NdcToViewOffset(glm::dvec2 ndc, double aspectRatio) const {
        return glm::dvec2(ndc.x * aspectRatio, ndc.y) / static_cast<double>(zoom);
    }
};
