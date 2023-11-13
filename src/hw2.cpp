#include "hw2.h"
#include "hw2_scenes.h"

using namespace hw2;

/**
 * Check if a point is inside a triangle
 * @param p0 The first vertex of the triangle
 * @param p1 The second vertex of the triangle
 * @param p2 The third vertex of the triangle
 * @param p The point to be checked
 * @return True if the point is inside the triangle, false otherwise
 */
bool is_inside_triangle(Vector2 p0, Vector2 p1, Vector2 p2, Vector2 p) {
    // Compute the edge vectors of the triangle
    Vector2 e01 = p1 - p0;
    Vector2 e12 = p2 - p1;
    Vector2 e20 = p0 - p2;

    // Rotate each edge vector by 90 degrees to obtain the normal vectors
    Vector2 n01(e01.y, -e01.x);
    Vector2 n12(e12.y, -e12.x);
    Vector2 n20(e20.y, -e20.x);

    // Compute dot products
    float d1 = dot(p - p0, n01);
    float d2 = dot(p - p1, n12);
    float d3 = dot(p - p2, n20);

    // Check if the point lies in the intersection of all positive or all negative half-planes
    return (d1 >= 0 && d2 >= 0 && d3 >= 0) || (d1 <= 0 && d2 <= 0 && d3 <= 0);
}

/**
 * Project a 3D point to 2D
 * @param p The 3D point to be projected
 * @return The projected 2D point
 */
Vector2 project(const Vector3 &p) {
    return { -p.x / p.z, -p.y / p.z };
}

/**
 * Convert a projected point to screen space
 * @param p The projected point
 * @param width The width of the screen
 * @param height The height of the screen
 * @param s The scaling factor of the view frustum
 * @return The point in screen space
 */
Vector2 toScreenSpace(const Vector2 &p, int width, int height, Real s) {
    float aspect_ratio = static_cast<float>(width) / height;

    return { width * (p.x + aspect_ratio * s) / (2 * aspect_ratio * s),
             height * (1 - (p.y + s) / (2 * s)) };  // y-axis is flipped
}

/**
 * Compute the barycentric coordinates of a point in a triangle
 * @param A The first vertex of the triangle
 * @param B The second vertex of the triangle
 * @param C The third vertex of the triangle
 * @param P The point to be checked
 * @return The barycentric coordinates of the point
 */
Vector3 barycentric_coordinates(const Vector2 &A, const Vector2 &B, const Vector2 &C, const Vector2 &P) {
    Real denom = (B.y - C.y) * (A.x - C.x) + (C.x - B.x) * (A.y - C.y);
    if (std::abs(denom) < 1e-6) {
        // Triangle is degenerate, return invalid barycentric coordinates
        return Vector3{-1, -1, -1};
    }

    Real b0 = ((B.y - C.y) * (P.x - C.x) + (C.x - B.x) * (P.y - C.y)) / denom;
    Real b1 = ((C.y - A.y) * (P.x - C.x) + (A.x - C.x) * (P.y - C.y)) / denom;
    Real b2 = 1.0 - b0 - b1;

    return Vector3{b0, b1, b2};
}

/**
 * Down-sample a super-sampled image
 * @param img The down-sampled image
 * @param superImg The super-sampled image
 * @param AA_FACTOR The anti-aliasing factor
 */
void down_sampled(Image3& img, const Image3& superImg, int AA_FACTOR) {
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            Vector3 sumColor = Vector3{0, 0, 0};
            for (int dy = 0; dy < AA_FACTOR; dy++) {
                for (int dx = 0; dx < AA_FACTOR; dx++) {
                    sumColor += superImg(x * AA_FACTOR + dx, y * AA_FACTOR + dy);
                }
            }
            img(x, y) = sumColor / Real(AA_FACTOR * AA_FACTOR);
        }
    }
}

/**
 * Transform a point by a matrix from Matrix4x4 to Vector3
 * @param m The transformation matrix
 * @param p The point to be transformed
 * @return The transformed point
 */
Vector3 transform_point(const Matrix4x4 &m, const Vector3 &p) {
    Vector4 p_homogeneous(p.x, p.y, p.z, 1.0);
    Vector4 p_transformed = m * p_homogeneous;
    return Vector3(p_transformed.x / p_transformed.w,
                   p_transformed.y / p_transformed.w,
                   p_transformed.z / p_transformed.w);
}

/**
 * Clip a triangle against the near clipping plane
 * @param p0 The first vertex of the triangle
 * @param p1 The second vertex of the triangle
 * @param p2 The third vertex of the triangle
 * @param z_near The distance of the near clipping plane
 * @return The clipped triangle vertices
 */
std::vector<Vector3> clipTriangle(const Vector3& p0, const Vector3& p1, const Vector3& p2, Real z_near) {
    // Helper lambda function to calculate intersection point between a line and the near clipping plane
    auto intersect = [&](const Vector3& p1, const Vector3& p2) {
        Real t = (z_near - p1.z) / (p2.z - p1.z);
        return Vector3(
                p1.x + t * (p2.x - p1.x),
                p1.y + t * (p2.y - p1.y),
                z_near
        );
    };

    // Check each vertex against the near clipping plane
    bool v0 = p0.z >= z_near;
    bool v1 = p1.z >= z_near;
    bool v2 = p2.z >= z_near;

    // Handle different cases
    if (v0 && v1 && v2) {
        // All vertices are in front of the near clipping plane
        return {p0, p1, p2};
    } else if (v0 && v1) {
        // p2 is behind the near clipping plane
        return {p0, p1, intersect(p0, p2), intersect(p1, p2)};
    } else if (v1 && v2) {
        // p0 is behind the near clipping plane
        return {p1, p2, intersect(p1, p0), intersect(p2, p0)};
    } else if (v0 && v2) {
        // p1 is behind the near clipping plane
        return {p0, p2, intersect(p0, p1), intersect(p2, p1)};
    } else if (v0) {
        // p1 and p2 are behind the near clipping plane
        return {p0, intersect(p0, p1), intersect(p0, p2)};
    } else if (v1) {
        // p0 and p2 are behind the near clipping plane
        return {p1, intersect(p1, p0), intersect(p1, p2)};
    } else if (v2) {
        // p0 and p1 are behind the near clipping plane
        return {p2, intersect(p2, p0), intersect(p2, p1)};
    } else {
        // All vertices are behind the near clipping plane
        return {};
    }
}

/**
 * Rendering a single 3D triangle
 * @param params The parameters of the scene
 * @return The rendered image
 */
Image3 hw_2_1(const std::vector<std::string> &params) {
    // Homework 2.1: render a single 3D triangle

    Image3 img(640 /* width */, 480 /* height */);

    const int AA_FACTOR = 4;
    const int SUPER_WIDTH = 640 * AA_FACTOR;
    const int SUPER_HEIGHT = 480 * AA_FACTOR;
    Image3 superImg(SUPER_WIDTH, SUPER_HEIGHT);

    Vector3 p0{0, 0, -1};
    Vector3 p1{1, 0, -1};
    Vector3 p2{0, 1, -1};
    Real s = 1; // scaling factor of the view frustrum
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Real z_near = 1e-6; // distance of the near clipping plane
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-p0") {
            p0.x = std::stof(params[++i]);
            p0.y = std::stof(params[++i]);
            p0.z = std::stof(params[++i]);
        } else if (params[i] == "-p1") {
            p1.x = std::stof(params[++i]);
            p1.y = std::stof(params[++i]);
            p1.z = std::stof(params[++i]);
        } else if (params[i] == "-p2") {
            p2.x = std::stof(params[++i]);
            p2.y = std::stof(params[++i]);
            p2.z = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        }
    }

    for (int y = 0; y < SUPER_HEIGHT; y++) {
        for (int x = 0; x < SUPER_WIDTH; x++) {
            superImg(x, y) = Vector3{0.5, 0.5, 0.5}; // Default background color
        }
    }



    // Project the 3D triangle vertices to 2D
    Vector2 p0_projected = project(p0);
    Vector2 p1_projected = project(p1);
    Vector2 p2_projected = project(p2);

    // Convert the projected vertices to super-sampled screen space
    Vector2 p0_screen = toScreenSpace(p0_projected, SUPER_WIDTH, SUPER_HEIGHT, s);
    Vector2 p1_screen = toScreenSpace(p1_projected, SUPER_WIDTH, SUPER_HEIGHT, s);
    Vector2 p2_screen = toScreenSpace(p2_projected, SUPER_WIDTH, SUPER_HEIGHT, s);

    int x_min = std::min({p0_screen.x, p1_screen.x, p2_screen.x});
    int x_max = std::max({p0_screen.x, p1_screen.x, p2_screen.x});
    int y_min = std::min({p0_screen.y, p1_screen.y, p2_screen.y});
    int y_max = std::max({p0_screen.y, p1_screen.y, p2_screen.y});

// Bounding box for fast rendering
    x_min = std::max(0, x_min);
    y_min = std::max(0, y_min);
    x_max = std::min(SUPER_WIDTH - 1, x_max);
    y_max = std::min(SUPER_HEIGHT - 1, y_max);


    for (int y = y_min; y <= y_max; y++) {
        for (int x = x_min; x <= x_max; x++) {
            if (is_inside_triangle(p0_screen, p1_screen, p2_screen, Vector2(x + 0.5, y + 0.5))) {
                superImg(x, y) = color;
            }
        }
    }

    // Downsampling
    down_sampled(img, superImg, AA_FACTOR);

    return img;
}

/**
 * triangle clipping
 * In practice, instead of rejecting a triangle if one or two verices are behind the near clipping plane, graphics pipelines would implement triangle clipping (Fig. 5). As a bonus, you will implement the clipping of the triangles and render them correctly even when some vertices are behind the near clipping plane.
 * @param params
 * @return
 */
Image3 hw_2_1_bonus(const std::vector<std::string>& params) {
    Image3 img(640 /* width */, 480 /* height */);

    const int AA_FACTOR = 4;
    const int SUPER_WIDTH = 640 * AA_FACTOR;
    const int SUPER_HEIGHT = 480 * AA_FACTOR;
    Image3 superImg(SUPER_WIDTH, SUPER_HEIGHT);

    Vector3 p0{0, 0, -1};
    Vector3 p1{1, 0, -1};
    Vector3 p2{0, 1, -1};
    Real s = 1; // scaling factor of the view frustrum
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Real z_near = 1e-6; // distance of the near clipping plane

    for (size_t i = 0; i < params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-p0") {
            p0.x = std::stof(params[++i]);
            p0.y = std::stof(params[++i]);
            p0.z = std::stof(params[++i]);
        } else if (params[i] == "-p1") {
            p1.x = std::stof(params[++i]);
            p1.y = std::stof(params[++i]);
            p1.z = std::stof(params[++i]);
        } else if (params[i] == "-p2") {
            p2.x = std::stof(params[++i]);
            p2.y = std::stof(params[++i]);
            p2.z = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        }
    }

    for (int y = 0; y < SUPER_HEIGHT; y++) {
        for (int x = 0; x < SUPER_WIDTH; x++) {
            superImg(x, y) = Vector3{0.5, 0.5, 0.5};
        }
    }

    // Clip the Triangle
    std::vector<Vector3> clipped_triangle = clipTriangle(p0, p1, p2, z_near);
    if (clipped_triangle.size() < 3) {
        // Triangle is completely behind the near clipping plane, return an empty image
        down_sampled(img, superImg, AA_FACTOR);
        return img;
    }

    // Update the triangle's vertices after clipping
    p0 = clipped_triangle[0];
    p1 = clipped_triangle[1];
    p2 = clipped_triangle[2];

    // Project the 3D triangle vertices to 2D
    Vector2 p0_projected = project(p0);
    Vector2 p1_projected = project(p1);
    Vector2 p2_projected = project(p2);

    // Convert the projected vertices to super-sampled screen space
    Vector2 p0_screen = toScreenSpace(p0_projected, SUPER_WIDTH, SUPER_HEIGHT, s);
    Vector2 p1_screen = toScreenSpace(p1_projected, SUPER_WIDTH, SUPER_HEIGHT, s);
    Vector2 p2_screen = toScreenSpace(p2_projected, SUPER_WIDTH, SUPER_HEIGHT, s);

    int x_min = std::min({p0_screen.x, p1_screen.x, p2_screen.x});
    int x_max = std::max({p0_screen.x, p1_screen.x, p2_screen.x});
    int y_min = std::min({p0_screen.y, p1_screen.y, p2_screen.y});
    int y_max = std::max({p0_screen.y, p1_screen.y, p2_screen.y});

    // Bounding box for fast rendering
    x_min = std::max(0, x_min);
    y_min = std::max(0, y_min);
    x_max = std::min(SUPER_WIDTH - 1, x_max);
    y_max = std::min(SUPER_HEIGHT - 1, y_max);

    for (int y = y_min; y <= y_max; y++) {
        for (int x = x_min; x <= x_max; x++) {
            if (is_inside_triangle(p0_screen, p1_screen, p2_screen, Vector2(x + 0.5, y + 0.5))) {
                superImg(x, y) = color;
            }
        }
    }

    // Downsampling
    down_sampled(img, superImg, AA_FACTOR);

    return img;
}

/**
 * Render a triangle mesh
 * @param params The parameters of the scene
 * @return The rendered image
 */
Image3 hw_2_2(const std::vector<std::string> &params) {
    // Homework 2.2: render a triangle mesh
    const int AA_FACTOR = 4;
    const int SUPER_WIDTH = 640 * AA_FACTOR;
    const int SUPER_HEIGHT = 480 * AA_FACTOR;

    Image3 superImg(SUPER_WIDTH, SUPER_HEIGHT);

    std::vector<Real> z_buffer(SUPER_WIDTH * SUPER_HEIGHT, -std::numeric_limits<Real>::infinity());

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];

    for (int y = 0; y < SUPER_HEIGHT; y++) {
        for (int x = 0; x < SUPER_WIDTH; x++) {
            superImg(x, y) = Vector3{0.5, 0.5, 0.5};
        }
    }

    // Render each triangle
    for (const auto &face : mesh.faces) {
        Vector3 v0 = mesh.vertices[face[0]];
        Vector3 v1 = mesh.vertices[face[1]];
        Vector3 v2 = mesh.vertices[face[2]];

        Vector2 projected_v0 = toScreenSpace(project(v0), SUPER_WIDTH, SUPER_HEIGHT, s);
        Vector2 projected_v1 = toScreenSpace(project(v1), SUPER_WIDTH, SUPER_HEIGHT, s);
        Vector2 projected_v2 = toScreenSpace(project(v2), SUPER_WIDTH, SUPER_HEIGHT, s);

        int x_min = std::min({projected_v0.x, projected_v1.x, projected_v2.x});
        int x_max = std::max({projected_v0.x, projected_v1.x, projected_v2.x});
        int y_min = std::min({projected_v0.y, projected_v1.y, projected_v2.y});
        int y_max = std::max({projected_v0.y, projected_v1.y, projected_v2.y});

        // bounding box for fast rendering
        x_min = std::max(0, x_min);
        y_min = std::max(0, y_min);
        x_max = std::min(SUPER_WIDTH - 1, x_max);
        y_max = std::min(SUPER_HEIGHT - 1, y_max);

        for (int y = y_min; y <= y_max; y++) {
            for (int x = x_min; x <= x_max; x++) {
                Vector2 pixel_center(x + 0.5, y + 0.5);
                if (is_inside_triangle(projected_v0, projected_v1, projected_v2, pixel_center)) {
                    Vector3 barycentric = barycentric_coordinates(projected_v0, projected_v1, projected_v2, pixel_center);

                    if (barycentric.x < 0 || barycentric.y < 0 || barycentric.z < 0) continue;

                    Real depth = barycentric.x * v0.z + barycentric.y * v1.z + barycentric.z * v2.z;

                    int z_index = y * SUPER_WIDTH + x;
                    if (depth > z_buffer[z_index]) {
                        superImg(x, y) = mesh.face_colors[&face - &mesh.faces[0]];
                        z_buffer[z_index] = depth;
                    }
                }
            }
        }
    }

    // Downsampling
    down_sampled(img, superImg, AA_FACTOR);
    return img;
}

/**
 * occlusion culling (Faster Rendering)
 * @param params The parameters of the scene
 * @return The rendered image
 */
Image3 hw_2_2_bonus(const std::vector<std::string> &params) {
    const int AA_FACTOR = 4;
    const int SUPER_WIDTH = 640 * AA_FACTOR;
    const int SUPER_HEIGHT = 480 * AA_FACTOR;

    Image3 superImg(SUPER_WIDTH, SUPER_HEIGHT);

    std::vector<Real> z_buffer(SUPER_WIDTH * SUPER_HEIGHT, -std::numeric_limits<Real>::infinity());

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];

    for (int y = 0; y < SUPER_HEIGHT; y++) {
        for (int x = 0; x < SUPER_WIDTH; x++) {
            superImg(x, y) = Vector3{0.5, 0.5, 0.5};
        }
    }
    for (const auto &face : mesh.faces) {
        Vector3 v0 = mesh.vertices[face[0]];
        Vector3 v1 = mesh.vertices[face[1]];
        Vector3 v2 = mesh.vertices[face[2]];

        Vector2 projected_v0 = toScreenSpace(project(v0), SUPER_WIDTH, SUPER_HEIGHT, s);
        Vector2 projected_v1 = toScreenSpace(project(v1), SUPER_WIDTH, SUPER_HEIGHT, s);
        Vector2 projected_v2 = toScreenSpace(project(v2), SUPER_WIDTH, SUPER_HEIGHT, s);

        int x_min = std::min({projected_v0.x, projected_v1.x, projected_v2.x});
        int x_max = std::max({projected_v0.x, projected_v1.x, projected_v2.x});
        int y_min = std::min({projected_v0.y, projected_v1.y, projected_v2.y});
        int y_max = std::max({projected_v0.y, projected_v1.y, projected_v2.y});

        // Bounding box for fast rendering
        x_min = std::max(0, x_min);
        y_min = std::max(0, y_min);
        x_max = std::min(SUPER_WIDTH - 1, x_max);
        y_max = std::min(SUPER_HEIGHT - 1, y_max);

        bool occluded = true;
        for (int y = y_min; y <= y_max && occluded; y++) {
            for (int x = x_min; x <= x_max && occluded; x++) {
                int z_index = y * SUPER_WIDTH + x;
                Real max_triangle_depth = std::max({v0.z, v1.z, v2.z});
                if (z_buffer[z_index] < max_triangle_depth) {
                    occluded = false;
                }
            }
        }

        if (occluded) {
            continue; // Skip rendering this triangle, it's completely occluded
        }

        // Rasterize and render the triangle
        for (int y = y_min; y <= y_max; y++) {
            for (int x = x_min; x <= x_max; x++) {
                Vector2 pixel_center(x + 0.5, y + 0.5);
                if (is_inside_triangle(projected_v0, projected_v1, projected_v2, pixel_center)) {
                    Vector3 barycentric = barycentric_coordinates(projected_v0, projected_v1, projected_v2, pixel_center);

                    if (barycentric.x < 0 || barycentric.y < 0 || barycentric.z < 0) continue;

                    Real depth = barycentric.x * v0.z + barycentric.y * v1.z + barycentric.z * v2.z;

                    int z_index = y * SUPER_WIDTH + x;
                    if (depth > z_buffer[z_index]) {
                        superImg(x, y) = mesh.face_colors[&face - &mesh.faces[0]];
                        z_buffer[z_index] = depth;
                    }
                }
            }
        }
    }

    // Downsampling
    down_sampled(img, superImg, AA_FACTOR);
    return img;
}

/**
 * Perspective-corrected interpolation
 * @param params The parameters of the scene
 * @return The rendered image
 */
Image3 hw_2_3(const std::vector<std::string> &params) {
    // Homework 2.3: render a triangle mesh with interpolated vertex colors and super-sampling

    const int AA_FACTOR = 4;
    const int SUPER_WIDTH = 640 * AA_FACTOR;
    const int SUPER_HEIGHT = 480 * AA_FACTOR;

    std::vector<Real> z_buffer(SUPER_WIDTH * SUPER_HEIGHT, -std::numeric_limits<Real>::infinity());

    Image3 superImg(SUPER_WIDTH, SUPER_HEIGHT);
    Image3 img(640, 480);

    Real s = 1; // scaling factor of the view frustrum
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];

    for (int y = 0; y < SUPER_HEIGHT; y++) {
        for (int x = 0; x < SUPER_WIDTH; x++) {
            superImg(x, y) = Vector3{0.5, 0.5, 0.5};
        }
    }

    // Render each triangle with interpolated vertex colors
    for (const auto &face : mesh.faces) {
        Vector3 v0 = mesh.vertices[face[0]];
        Vector3 v1 = mesh.vertices[face[1]];
        Vector3 v2 = mesh.vertices[face[2]];

        Vector3 color0 = mesh.vertex_colors[face[0]];
        Vector3 color1 = mesh.vertex_colors[face[1]];
        Vector3 color2 = mesh.vertex_colors[face[2]];

        Vector2 projected_v0 = toScreenSpace(project(v0), SUPER_WIDTH, SUPER_HEIGHT, s);
        Vector2 projected_v1 = toScreenSpace(project(v1), SUPER_WIDTH, SUPER_HEIGHT, s);
        Vector2 projected_v2 = toScreenSpace(project(v2), SUPER_WIDTH, SUPER_HEIGHT, s);

        int x_min = std::min({projected_v0.x, projected_v1.x, projected_v2.x});
        int x_max = std::max({projected_v0.x, projected_v1.x, projected_v2.x});
        int y_min = std::min({projected_v0.y, projected_v1.y, projected_v2.y});
        int y_max = std::max({projected_v0.y, projected_v1.y, projected_v2.y});

        // bounding box for fast rendering
        x_min = std::max(0, x_min);
        y_min = std::max(0, y_min);
        x_max = std::min(SUPER_WIDTH - 1, x_max);
        y_max = std::min(SUPER_HEIGHT - 1, y_max);

        for (int y = y_min; y <= y_max; y++) {
            for (int x = x_min; x <= x_max; x++) {
                Vector2 pixel_center(x + 0.5, y + 0.5);
                if (is_inside_triangle(projected_v0, projected_v1, projected_v2, pixel_center)) {
                    Vector3 barycentric = barycentric_coordinates(projected_v0, projected_v1, projected_v2,
                                                                  pixel_center);

                    if (barycentric.x < 0 || barycentric.y < 0 || barycentric.z < 0) continue;

                    Real depth = barycentric.x * v0.z + barycentric.y * v1.z + barycentric.z * v2.z;

                    int z_index = y * SUPER_WIDTH + x;
                    if (depth > z_buffer[z_index]) {
                        Vector3 interpolated_color =
                                barycentric.x * color0 + barycentric.y * color1 + barycentric.z * color2;
                        superImg(x, y) = interpolated_color;
                        z_buffer[z_index] = depth;
                    }
                }
            }
        }
    }

    // Downsampling
    down_sampled(img, superImg, AA_FACTOR);

    return img;
}

/**
 * 3D transformation
 * @param params The parameters of the scene
 * @return The rendered image
 */
Image3 hw_2_4(const std::vector<std::string> &params) {
    // Homework 2.4: render a scene with transformation
    if (params.size() == 0) {
        return Image3(0, 0);
    }


    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.camera.resolution.x,
               scene.camera.resolution.y);

    const int AA_FACTOR = 4;
    const int SUPER_WIDTH = scene.camera.resolution.x * AA_FACTOR;
    const int SUPER_HEIGHT = scene.camera.resolution.y * AA_FACTOR;

    Image3 superImg(SUPER_WIDTH, SUPER_HEIGHT);

    std::vector<Real> z_buffer(SUPER_WIDTH * SUPER_HEIGHT, -std::numeric_limits<Real>::infinity());

    for (int y = 0; y < SUPER_HEIGHT; y++) {
        for (int x = 0; x < SUPER_WIDTH; x++) {
            superImg(x, y) = Vector3{scene.background[0], scene.background[1], scene.background[2]}; // Default background color
        }
    }

    for (const auto &mesh : scene.meshes) {
        for (const auto &face: mesh.faces) {
            Vector3 v0 = mesh.vertices[face[0]];
            Vector3 v1 = mesh.vertices[face[1]];
            Vector3 v2 = mesh.vertices[face[2]];

            Vector3 color0 = mesh.vertex_colors[face[0]];
            Vector3 color1 = mesh.vertex_colors[face[1]];
            Vector3 color2 = mesh.vertex_colors[face[2]];

            // calculate the model view matrix
            Matrix4x4 view = inverse(scene.camera.cam_to_world);
            Matrix4x4 model_view = view * mesh.model_matrix;

            Vector2 projected_v0 = toScreenSpace(project(transform_point(model_view, v0)), SUPER_WIDTH, SUPER_HEIGHT, scene.camera.s);
            Vector2 projected_v1 = toScreenSpace(project(transform_point(model_view, v1)), SUPER_WIDTH, SUPER_HEIGHT, scene.camera.s);
            Vector2 projected_v2 = toScreenSpace(project(transform_point(model_view, v2)), SUPER_WIDTH, SUPER_HEIGHT, scene.camera.s);

            int x_min = std::min({projected_v0.x, projected_v1.x, projected_v2.x});
            int x_max = std::max({projected_v0.x, projected_v1.x, projected_v2.x});
            int y_min = std::min({projected_v0.y, projected_v1.y, projected_v2.y});
            int y_max = std::max({projected_v0.y, projected_v1.y, projected_v2.y});

            // bounding box for fast rendering
            x_min = std::max(0, x_min);
            y_min = std::max(0, y_min);
            x_max = std::min(SUPER_WIDTH - 1, x_max);
            y_max = std::min(SUPER_HEIGHT - 1, y_max);

            for (int y = y_min; y <= y_max; y++) {
                for (int x = x_min; x <= x_max; x++) {
                    Vector2 pixel_center(x + 0.5, y + 0.5);
                    if (is_inside_triangle(projected_v0, projected_v1, projected_v2, pixel_center)) {
                        Vector3 barycentric = barycentric_coordinates(projected_v0, projected_v1, projected_v2,
                                                                      pixel_center);

                        if (barycentric.x < 0 || barycentric.y < 0 || barycentric.z < 0) continue;

                        Real depth = barycentric.x * v0.z + barycentric.y * v1.z + barycentric.z * v2.z;

                        int z_index = y * SUPER_WIDTH + x;
                        if (depth > z_buffer[z_index]) {
                            Vector3 interpolated_color =
                                    barycentric.x * color0 + barycentric.y * color1 + barycentric.z * color2;
                            superImg(x, y) = interpolated_color;
                            z_buffer[z_index] = depth;
                        }
                    }
                }
            }
        }
    }

    down_sampled(img, superImg, AA_FACTOR);
    return img;
}

/**
 * Dummy function. Rendering my own scene. Call hw_2_4 inside this function.
 * @param params Given parameters
 * @return my nice scene
 */
Image3 hw_2_5(const std::vector<std::string> &params) {
    return hw_2_4(params);
}

