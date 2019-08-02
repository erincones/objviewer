#ifndef __CAMERA_HPP_
#define __CAMERA_HPP_

#include "../scene/glslprogram.hpp"

#include <glm/mat4x4.hpp>
#include <glm/vec3.hpp>


class Camera {
    private:
        // Attributes

        /** Orthogonal projection flag */
        bool orthogonal;


        /** Position */
        glm::vec3 position;

        /** View direction */
        glm::vec3 front;

        /** Right vector */
        glm::vec3 right;

        /** World up */
        glm::vec3 up;


        /** Resolution with */
        int width;

        /** Resolution height */
        int height;

        /** Field of view */
        float fov;

        /** Plane z near */
        float near;

        /** Plane z far */
        float far;


        /** View matrix */
        glm::mat4 view_mat;

        /** Orthogonal projection matrix */
        glm::mat4 orthogonal_mat;

        /** Perspective projection matrix */
        glm::mat4 perspective_mat;


        /** Rotation  pitch */
        float pitch;

        /** Rotation yaw */
        float yaw;


        // Constructors

        /** Delete the default constructor */
        Camera() = delete;


        // Methods

        /** updateViewMatrix */
        void updateViewMatrix();

        /** Update projection matrices */
        void updateProjectionMatrices();


        // Static attributes

        /** Boost flag */
        static bool boosted;

        /** Normal speed */
        static float speed;

        /** Boosted speed */
        static float boosted_speed;


        /** Mouse sensibility */
        static float sensibility;


        /** Zoom factor */
        static float zoom_factor;


    public:
        // Enumerations

        /** Movement in the front-right plane */
        enum Movement {
            /** Right */
            RIGHT,

            /** Left */
            LEFT,

            /** Up */
            UP,

            /** Down */
            DOWN,

            /** Front */
            FRONT,

            /** Back */
            BACK
        };


        // Constructor

        /** Camera constructor */
        Camera(const int &width, const int &height, const bool &orthogonal = false);


        // Getters

        /** Get the orthogonal projection status */
        bool isOrthogonal() const;


        /** Get position */
        glm::vec3 getPosition() const;

        /** Get look direction */
        glm::vec3 getDirection() const;

        /** Get look angles */
        glm::vec3 getRotation() const;

        /** Get up vector */
        glm::vec3 getUp() const;


        /** Get field of view */
        float getFOV() const;

        /** Get resolution */
        glm::vec2 getResolution() const;

        /** Get clipping */
        glm::vec2 getClipping() const;


        /** Get the view matrix */
        glm::mat4 getViewMatrix() const;

        /** Get the orthogonal projection matrix */
        glm::mat4 getOrthogonalMatrix() const;

        /** Get the perspective projection matrix */
        glm::mat4 getPerspectiveMatrix() const;

        /** Get the current projection matrix */
        glm::mat4 getProjectionMatrix() const;


        // Setters

        /** Set the orthogonal projection status */
        void isOrthogonal(const bool &status);


        /** Set position */
        void setPosition(const glm::vec3 &new_position);

        /** Set look direction */
        void setDirection(const glm::vec3 &direction);

        /** Set look angles */
        void setRotation(const glm::vec3 &angle);

        /** Set up vector */
        void setUp(const glm::vec3 &direction);


        /** Set field of view */
        void setFOV(const float &new_fov);

        /** Set resolution */
        void setResolution(const glm::vec2 &resolution);

        /** Set clipping */
        void setClipping(const glm::vec2 &clipping);


        // Methods

        /** Reset to default values */
        void reset();


        /** Bind the camera */
        void bind(GLSLProgram *const program) const;


        /** Travell the camera */
        void travell(const Camera::Movement &direction, const double &time = 1.0 / 30.0);

        /** Translate the camera */
        void translate(const glm::vec3 &delta);

        /** Apply zoom */
        void zoom(const double &direction);


        /** Rotate the camera */
        void rotate(const glm::vec2 &delta);


        // Destructor

        /** Camera destructor */
        virtual ~Camera();


        // Static getters

        /** Get the boost status */
        static bool isBoosted();

        /** Get the normal speed */
        static float getSpeed();

        /** Get the boosted speed */
        static float getBoostedSpeed();


        /** Get mouse sensibility */
        static float getSensibility();


        /** Get the zoom factor */
        static float getZoomFactor();


        // Static setters

        /** Set the boost status */
        static void setBoosted(const bool &status);

        /** Set the normal speed */
        static void setSpeed(const float &speed);

        /** Set the boosted speed */
        static void setBoostedSpeed(const float &speed);


        /** Set the mouse sensibility */
        static void setSensibility(const float &sesibility);


        /** Set the zoom factor */
        static void setZoomFactor(const float &factor);
};

#endif // __CAMERA_HPP_