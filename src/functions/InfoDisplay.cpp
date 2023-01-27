#include "InfoDisplay.h"

#include <mc/Actor.hpp>
// #include <mc/BaseCircuitComponent.hpp>
#include <mc/Biome.hpp>
#include <mc/Block.hpp>
#include <mc/BlockActor.hpp>
#include <mc/Brightness.hpp>
#include <mc/CircuitSystem.hpp>
#include <mc/Material.hpp>
#include <mc/NavigationComponent.hpp>
#include <mc/Player.hpp>

#include "CommandHelper.h"
#include "DataConverter.h"
#include "Global.h"
#include "MC/PrettySnbtFormat.hpp"
#include "Msg.h"
#include "TBlockPos.h"
#include "Utils.h"
namespace trapdoor {
    namespace {
        std::string printableNBT(const std::unique_ptr<CompoundTag> &nbt) {
            return nbt->toPrettySNBT(true);
        }

        ActionResult getNBTString(const std::unique_ptr<CompoundTag> &nbt,
                                  const std::string &path) {
            if (path.empty()) {
                return {printableNBT(nbt), true};
            } else {
                bool success = false;
                auto str = getNBTInfoFromPath(nbt, path, success);
                return {str, success};
            }
        }
    }  // namespace
    ActionResult displayEntityInfo(Player *player, Actor *a, bool nbt, const std::string &path) {
        if (!player) return ErrorPlayerNeed();
        if (!a) {
            return ErrorMsg("data.error.no-actor");
        }
        TextBuilder builder;
        if (nbt) {
            return getNBTString(a->getNbt(), path);
        }

        builder.sText(TextBuilder::AQUA, "Base: \n")
            .text(" - type / UID: ")
            .sTextF(TextBuilder::GREEN, "%s    %llx\n", a->getTypeName().c_str(),
                    a->getUniqueID().get())
            .text(" - Position: ")
            .sTextF(TextBuilder::GREEN, "%s\n", fromVec3(a->getPos()).toDetailString().c_str())
            .text(" - DeltaPos: ")
            .sTextF(TextBuilder::GREEN, "%s\n", fromVec3(a->getPosDelta()).toDetailString().c_str())
            .text(" - AABB: ")
            .sTextF(TextBuilder::GREEN, "%s\n", fromAABB(a->getAABB()).ToString().c_str())
            .text(" - Surface: ")
            .sTextF(TextBuilder::GREEN, "%d\n", a->isSurfaceMob());
        return {builder.get(), true};
    }

    ActionResult displayBlockInfo(Player *p, const BlockPos &position, bool nbt,
                                  const std::string &path) {
        if (!p) return ErrorPlayerNeed();
        auto pos = position;
        if (pos == BlockPos::MAX) {
            pos = trapdoor::getLookAtPos(p);
        }

        if (pos == BlockPos::MAX) {
            return ErrorMsg("data.error.no-block");
        }

        auto &b = p->getRegion().getBlock(pos);
        trapdoor::TextBuilder builder;
        if (nbt) {
            if (b.hasBlockEntity()) {
                auto be = p->getRegion().getBlockEntity(pos);
                if (be) {
                    return getNBTString(be->getNbt(), path);
                }
            } else {
                return ErrorMsg("data.error.no-nbt");
            }
        }

        builder.sText(trapdoor::TextBuilder::AQUA, "Base:\n")
            .text(" - Ptr: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%p\n", &b)
            .text(" - Name / Type: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%s / %s\n", b.getName().c_str(),
                    b.getTypeName().c_str())

            .text(" - ID / RTID: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d / %d\n", b.getId(), b.getRuntimeId())
            .text(" - Variant: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", b.getVariant())
            .text(" - CanInstanceTick: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", b.canInstatick())
            .text(" - BlockEntity: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", b.hasBlockEntity())
            .text(" - IsSolid: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", b.isSolid());
        auto &m = b.getMaterial();
        builder.sText(trapdoor::TextBuilder::AQUA, "Material:\n")
            .text(" - Motion: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", m.getBlocksMotion())
            .text(" - TopSolid: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", m.isTopSolid(false, false))
            .text(" - IsSolid: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", m.isSolid())
            .text(" - IsSolidBlocking: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%d\n", m.isSolidBlocking())
            .text(" - Translucency: ")
            .sTextF(trapdoor::TextBuilder::GREEN, "%.3f\n", m.getTranslucency());
        return {builder.get(), true};
    }

    bool displayEnvInfo() { return true; }

}  // namespace trapdoor
