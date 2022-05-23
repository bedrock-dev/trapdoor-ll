#include "Particle.h"

#include <MC/Level.hpp>
#include <MC/Vec3.hpp>
#include <array>
#include <map>

#include "TBlockPos.h"
#include "TVec3.h"

namespace tr {

    namespace {

        const std::string PARTICLE_BASE_NAME = "traodoor::line";

        std::array<std::string, 6>& lineParticleFacing() {
            static std::array<std::string, 6> facing{"Yp", "Ym", "Zp",
                                                     "Zm", "Xp", "Xm"};
            return facing;
        }

        std::array<std::string, 5>& lineParticleColor() {
            //  enum class PCOLOR { WHITE = 0, RED = 1, YELLOW = 2, BLUE = 3,
            //  GREEN = 4 };
            static std::array<std::string, 5> color{"W", "R", "Y", "B", "G"};
            return color;
        }

        //把整数进行二进制分割，获取粒子的生成坐标
        std::map<float, int> binSplit(float start, float end) {
            std::map<float, int> lengthMap;
            int length = static_cast<int>(end - start);
            while (length >= 512) {
                length -= 256;
                auto point = static_cast<float>(128.0 + start);
                start += 256.0;
                lengthMap.insert({point, 256});
            }

            for (auto defaultLength = 256; defaultLength >= 1;
                 defaultLength /= 2) {
                if (length >= defaultLength) {
                    length -= defaultLength;
                    auto point =
                        static_cast<float>(0.5 * defaultLength + start);
                    start += defaultLength;
                    lengthMap.insert({point, defaultLength});
                }
            }
            return lengthMap;
        }

    }  // namespace
       //对外使用tr自己的vec3，调api时使用自己的
    void SpawnParticle(const TVec3& pos, const std::string& type, int dimID) {
        Vec3 p(pos.x, pos.y, pos.z);
        Level::spawnParticleEffect(
            type, p,
            Level::getDimension(
                static_cast<AutomaticID<Dimension, int> >(dimID)));
    }

    std::string buildLienParticleType(int length, TFACING direction,
                                      PCOLOR color, bool back = false) {
        std::string str = "trapdoor:line";
        if (back) str += "_back";
        str += std::to_string(length);
        return str + lineParticleFacing()[static_cast<int>(direction)] +
               lineParticleColor()[static_cast<int>(color)];
    }

    void DrawLine(const TVec3& originPoint, TFACING direction, float length,
                  PCOLOR color, int dimType) {
        if (length <= 0) return;
        float start = 0, end = 0;
        switch (direction) {
            case TFACING::NEG_Y:
                start = originPoint.y - length;
                end = originPoint.y;
                break;
            case TFACING::POS_Y:
                start = originPoint.y;
                end = originPoint.y + length;
                break;
            case TFACING::NEG_Z:
                start = originPoint.z - length;
                end = originPoint.z;
                break;
            case TFACING::POS_Z:
                start = originPoint.z;
                end = originPoint.z + length;
                break;
            case TFACING::NEG_X:
                start = originPoint.x - length;
                end = originPoint.x;
                break;
            case TFACING::POS_X:
                start = originPoint.x;
                end = originPoint.x + length;
                break;
        }
        // split point list;
        auto list = binSplit(start, end);
        std::map<TVec3, int> positionList;
        if (facingIsX(direction)) {
            for (auto i : list)
                positionList.insert(
                    {{i.first, originPoint.y, originPoint.z}, i.second});
        } else if (facingIsY(direction)) {
            for (auto i : list)
                positionList.insert(
                    {{originPoint.x, i.first, originPoint.z}, i.second});
        } else if (facingIsZ(direction)) {
            for (auto i : list)
                positionList.insert(
                    {{originPoint.x, originPoint.y, i.first}, i.second});
        }

        for (auto points : positionList) {
            auto particleType =
                buildLienParticleType(points.second, direction, color, false);
            auto backParticleType =
                buildLienParticleType(points.second, direction, color, true);
            auto particleTypeInv = buildLienParticleType(
                points.second, invFacing(direction), color, false);
            auto backParticleTypeInv = buildLienParticleType(
                points.second, invFacing(direction), color, true);

            SpawnParticle(points.first, particleType, dimType);
            SpawnParticle(points.first, backParticleType, dimType);
            SpawnParticle(points.first, particleTypeInv, dimType);
            SpawnParticle(points.first, backParticleTypeInv, dimType);
        }
    }

    void DrawAABB(const TAABB& aabb, PCOLOR color, bool mark, int dimType = 0) {
        auto p1 = aabb.p1, p2 = aabb.p2;
        auto dx = p2.x - p1.x;
        auto dy = p2.y - p1.y;
        auto dz = p2.z - p1.z;
        DrawLine(p1, TFACING::POS_X, dx, color, dimType);
        if (mark) {
            DrawLine(p1, TFACING::POS_Y, dy, PCOLOR::WHITE, dimType);
        } else {
            DrawLine(p1, TFACING::POS_Y, dy, color, dimType);
        }
        DrawLine(p1, TFACING::POS_Z, dz, color, dimType);

        TVec3 p3{p2.x, p1.y, p2.z};
        DrawLine(p3, TFACING::NEG_X, dx, color, dimType);
        DrawLine(p3, TFACING::POS_Y, dy, color, dimType);
        DrawLine(p3, TFACING::NEG_Z, dz, color, dimType);
        TVec3 p4{p2.x, p2.y, p1.z};
        DrawLine(p4, TFACING::NEG_X, dx, color, dimType);
        DrawLine(p4, TFACING::NEG_Y, dy, color, dimType);
        DrawLine(p4, TFACING::POS_Z, dz, color, dimType);

        TVec3 p5{p1.x, p2.y, p2.z};
        DrawLine(p5, TFACING::POS_X, dx, color, dimType);
        DrawLine(p5, TFACING::NEG_Y, dy, color, dimType);
        DrawLine(p5, TFACING::NEG_Z, dz, color, dimType);
    }
}  // namespace tr