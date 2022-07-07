#include "InfoDisplay.h"

#include <MC/Actor.hpp>
#include <MC/BaseCircuitComponent.hpp>
#include <MC/Biome.hpp>
#include <MC/Block.hpp>
#include <MC/BlockActor.hpp>
#include <MC/BlockSource.hpp>
#include <MC/Brightness.hpp>
#include <MC/CircuitSceneGraph.hpp>
#include <MC/CircuitSystem.hpp>
#include <MC/Dimension.hpp>
#include <MC/Material.hpp>
#include <MC/NavigationComponent.hpp>
#include <MC/Player.hpp>
#include <vector>

#include "CommandHelper.h"
#include "DataConverter.h"
#include "GlobalServiceAPI.h"
#include "MC/PrettySnbtFormat.hpp"
#include "Msg.h"
#include "TrAPI.h"
#include "TrapdoorMod.h"
#include "Utils.h"
namespace tr {

    namespace {

        struct ActorNBTFormat : PrettySnbtFormat {
            ActorNBTFormat() {
                this->switchToPlayerFormat();
                this->mMaxLevel = 1;
            }
        };

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
            return {"No actor", false};
        }
        TextBuilder builder;
        if (nbt) {
            return getNBTString(a->getNbt(), path);
        }

        builder.sText(TextBuilder::AQUA, "Base: \n")
            .text(" - type / UID: ")
            .sTextF(TextBuilder::GREEN, "%s    %llx\n", a->getTypeName().c_str(),
                    a->getUniqueID().get())
            .text(" - Pos/DeltaPos: ")
            .sTextF(TextBuilder::GREEN, "%s / %s|%s", fromVec3(a->getPos()).toString().c_str(),
                    fromVec3(a->getPosDelta()).toString().c_str())
            .text(" - AABB: ")
            .sTextF(TextBuilder::GREEN, "%s", fromAABB(a->getAABB()).ToString().c_str())
            .text("\n")
            .text(" - Surface: ")
            .sTextF(TextBuilder::GREEN, "%d\n", a->isSurfaceMob());
        return {builder.get(), true};
    }

    ActionResult displayBlockInfo(Player *p, const BlockPos &position, bool nbt,
                                  const std::string &path) {
        if (!p) return ErrorPlayerNeed();
        auto pos = position;
        if (pos == BlockPos::MAX) {
            pos = tr::getLookAtPos(p);
        }

        if (pos == BlockPos::MAX) {
            return {"Get blockName failure", false};
        }
        auto &b = p->getRegion().getBlock(pos);
        tr::TextBuilder builder;
        if (nbt) {
            if (b.hasBlockEntity()) {
                auto be = p->getRegion().getBlockEntity(pos);
                if (be) {
                    return getNBTString(be->getNbt(), path);
                }
            } else {
                return {"No NBT data", false};
            }
        }

        builder.sText(tr::TextBuilder::AQUA, "Base:\n")
            .text(" - Name / Type: ")
            .sTextF(tr::TextBuilder::GREEN, "%s / %s\n", b.getName().c_str(),
                    b.getTypeName().c_str())
            .text(" - ID / RTID: ")
            .sTextF(tr::TextBuilder::GREEN, "%d / %d\n", b.getId(), b.getRuntimeId())
            .text(" - Variant: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", b.getVariant())
            .text(" - CanInstatick: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", b.canInstatick())
            .text(" - BlockEntity: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", b.hasBlockEntity())
            .text(" - IsSolid: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", b.isSolid());
        auto &m = b.getMaterial();
        builder.sText(tr::TextBuilder::AQUA, "Material:\n")
            .text(" - Motion: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", m.getBlocksMotion())
            .text(" - TopSolid: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", m.isTopSolid(false, false))
            .text(" - IsSolid: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", m.isSolid())
            .text(" - IsSolidBlocking: ")
            .sTextF(tr::TextBuilder::GREEN, "%d\n", m.isSolidBlocking())
            .text(" - Translucency: ")
            .sTextF(tr::TextBuilder::GREEN, "%.3f\n", m.getTranslucency());
        return {builder.get(), true};
    }

    bool displayEnvInfo() { return true; }

    ActionResult displayRedstoneCompInfo(Dimension *d, const BlockPos &pos) {
        if (!d) return ErrorDimension();
        auto &cs = d->getCircuitSystem();
        auto &graph = getCircuitSceneGraph(&cs);
        auto comp = graph.getBaseComponent(pos);
        if (!comp) {
            return {"Not an redstone component", false};
        }
        return {"Strength =>  " + std::to_string(comp->getStrength()), true};
    }
}  // namespace tr
