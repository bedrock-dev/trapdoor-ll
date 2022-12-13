#include "Particle.h"

#include <array>
#include <map>
#include <mc/Dimension.hpp>
#include <mc/Vec3.hpp>

#include "Configuration.h"
#include "Global.h"
#include "TBlockPos.h"
#include "TVec3.h"
#include "Utils.h"

namespace trapdoor {
    namespace {

        const std::string PARTICLE_BASE_NAME = "trapdoor::line";

        std::array<std::string, 6>& lineParticleFacing() {
            static std::array<std::string, 6> facing{"pY", "mY", "pZ", "mZ", "pX", "mX"};
            return facing;
        }

        std::array<std::string, 16>& lineParticleColor() {
            static std::array<std::string, 16> color{"B", "I", "L", "T", "C", "D", "O", "W",
                                                     "R", "A", "Y", "G", "V", "S", "P", "E"};
            return color;
        }

        // 把整数进行二进制分割，获取粒子的生成坐标
        std::map<float, int> binSplit(float start, float end) {
            std::map<float, int> lengthMap;
            int length = static_cast<int>(end - start);
            while (length >= 512) {
                length -= 256;
                auto point = static_cast<float>(128.0 + start);
                start += 256.0;
                lengthMap.insert({point, 256});
            }

            for (auto defaultLength = 256; defaultLength >= 1; defaultLength /= 2) {
                if (length >= defaultLength) {
                    length -= defaultLength;
                    auto point = static_cast<float>(0.5 * defaultLength + start);
                    start += static_cast<float>(defaultLength);
                    lengthMap.insert({point, defaultLength});
                }
            }
            return lengthMap;
        }

    }  // namespace
       // 对外使用tr自己的vec3，调api时使用自己的
    void spawnParticle(const TVec3& pos, const std::string& type, int dimID) {
        Vec3 p(pos.x, pos.y, pos.z);
        auto dim_ref = Global<Level>->getDimension(dimID).mHandle.lock();
        auto* d = trapdoor::unwrap_shard_ptr_ref(dim_ref.get());
        auto pvd = trapdoor::mod().getConfig().getBasicConfig().particleViewDistance2D;
        if (d->distanceToNearestPlayerSqr2D(p) > pvd) return;
        Global<Level>->spawnParticleEffect(type, p, d);
    }

    std::string buildLienParticleType(int length, TFACING direction, PCOLOR color,
                                      bool back = false) {
        std::string str = "ll:line";
        if (back) str += "_back";
        str += lineParticleFacing()[static_cast<int>(direction)];
        str += lineParticleColor()[static_cast<int>(color)];
        str += std::to_string(length);
        //  trapdoor::logger().debug("Particle name:{}", str);
        return str;
    }

    void drawLine(const TVec3& originPoint, TFACING direction, float length, PCOLOR color,
                  int dimType) {
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
                positionList.insert({{i.first, originPoint.y, originPoint.z}, i.second});
        } else if (facingIsY(direction)) {
            for (auto i : list)
                positionList.insert({{originPoint.x, i.first, originPoint.z}, i.second});
        } else if (facingIsZ(direction)) {
            for (auto i : list)
                positionList.insert({{originPoint.x, originPoint.y, i.first}, i.second});
        }

        for (auto points : positionList) {
            auto particleType = buildLienParticleType(points.second, direction, color, false);
            auto backParticleType = buildLienParticleType(points.second, direction, color, true);
            auto particleTypeInv =
                buildLienParticleType(points.second, invFacing(direction), color, false);
            auto backParticleTypeInv =
                buildLienParticleType(points.second, invFacing(direction), color, true);

            auto basicCfg = trapdoor::mod().getConfig().getBasicConfig();

            spawnParticle(points.first, particleType, dimType);
            if (basicCfg.particleLevel > 1) {
                spawnParticle(points.first, backParticleType, dimType);
                if (basicCfg.particleLevel > 2) {
                    spawnParticle(points.first, particleTypeInv, dimType);
                    spawnParticle(points.first, backParticleTypeInv, dimType);
                }
            }
        }
    }

    void drawAABB(const TAABB& aabb, PCOLOR color, bool mark, int dimType = 0) {
        auto p1 = aabb.p1, p2 = aabb.p2;
        auto dx = p2.x - p1.x;
        auto dy = p2.y - p1.y;
        auto dz = p2.z - p1.z;
        drawLine(p1, TFACING::POS_X, dx, color, dimType);
        if (mark) {
            drawLine(p1, TFACING::POS_Y, dy, PCOLOR::WHITE, dimType);
        } else {
            drawLine(p1, TFACING::POS_Y, dy, color, dimType);
        }
        drawLine(p1, TFACING::POS_Z, dz, color, dimType);

        TVec3 p3{p2.x, p1.y, p2.z};
        drawLine(p3, TFACING::NEG_X, dx, color, dimType);
        drawLine(p3, TFACING::POS_Y, dy, color, dimType);
        drawLine(p3, TFACING::NEG_Z, dz, color, dimType);
        TVec3 p4{p2.x, p2.y, p1.z};
        drawLine(p4, TFACING::NEG_X, dx, color, dimType);
        drawLine(p4, TFACING::NEG_Y, dy, color, dimType);
        drawLine(p4, TFACING::POS_Z, dz, color, dimType);

        TVec3 p5{p1.x, p2.y, p2.z};
        drawLine(p5, TFACING::POS_X, dx, color, dimType);
        drawLine(p5, TFACING::NEG_Y, dy, color, dimType);
        drawLine(p5, TFACING::NEG_Z, dz, color, dimType);
    }

    void shortHighlightBlock(const TBlockPos& pos, PCOLOR color, int dimType) {
        auto p1 = pos.toVec3();
        auto aabb = TAABB(p1, p1 + TVec3(1, 1, 1));
        drawAABB(aabb, color, false, dimType);
    }
    void drawChunkSurface(const TBlockPos2& p, int dimType) {
        //        bool isSlime = p.isSlimeChunk();
        //        std::string pName2 = isSlime ? "trapdoor:chunkslimep" : "trapdoor:chunkp";
        //        std::string pName1 = isSlime ? "trapdoor:chunkslimem" : "trapdoor:chunkm";
        std::string pName2 = "trapdoor:chunkp";
        std::string pName1 = "trapdoor:chunkm";

        auto x = static_cast<float>(p.x) * 16.0f;
        auto z = static_cast<float>(p.z) * 16.0f;
        TVec3 p1{x + 0.01f, 128.0f, z + 8.0f};
        TVec3 p2{x + 15.99f, 128.0f, z + 8.0f};
        TVec3 p3{x + 8.0f, 128.0f, z + 0.01f};
        TVec3 p4{x + 8.0f, 128.0f, z + 15.99f};
        spawnParticle(p1, pName1, dimType);
        spawnParticle(p2, pName1, dimType);
        spawnParticle(p3, pName2, dimType);
        spawnParticle(p4, pName2, dimType);
    }

    void spawnSlimeChunkParticle(const ChunkPos& p) {
        float x = static_cast<float>(p.x) * 16.0f;
        float z = static_cast<float>(p.z) * 16.0f;
        TVec3 p1{x + 0.01f, 0.0f, z + 8.0f};
        TVec3 p2{x + 15.99f, 0.0f, z + 8.0f};
        TVec3 p3{x + 8.0f, 0.0f, z + 0.01f};
        TVec3 p4{x + 8.0f, 0.0f, z + 15.99f};
        TVec3 top{x + 8.0f, 128.0f, z + 8.0f};
        std::string pName1 = "trapdoor:slime_side1";
        std::string pName2 = "trapdoor:slime_side2";
        std::string pName3 = "trapdoor:slime_top";
        spawnParticle(p1, pName1);
        spawnParticle(p2, pName1);
        spawnParticle(p3, pName2);
        spawnParticle(p4, pName2);
        spawnParticle(top, pName3);
    }
    void spawnNumParticle(const TVec3& v, int num, PCOLOR color, int dimType) {
        auto name = "ll:num" + std::to_string(num) + lineParticleColor()[color];
        spawnParticle(v, name, dimType);
    }
}  // namespace trapdoor