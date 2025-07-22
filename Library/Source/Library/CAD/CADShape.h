#pragma once

#include <memory>
#include <string>
#include <vector>
#include <glm/glm.hpp>

class TopoDS_Shape;

namespace Library
{

    class CADShape
    {
      public:
        CADShape();
        virtual ~CADShape();

        virtual std::string toString() const;
        virtual std::string getType() const;

        const std::vector<std::shared_ptr<CADShape>>& getChildren() const;
        void                                          addChild(std::unique_ptr<CADShape>);

        std::pair<glm::dvec3, glm::dvec3>      getBoundingBox() const;

        bool                             save(const std::string& filename) const;
        static std::unique_ptr<CADShape> load(const std::string& filename);

        TopoDS_Shape&       getData();
        const TopoDS_Shape& getData() const;
        void                setData(const TopoDS_Shape&);

      private:
        std::unique_ptr<TopoDS_Shape> data = nullptr;

        std::vector<std::shared_ptr<CADShape>> children;
    };
}