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

#include "DataConverter.h"
#include "GlobalServiceAPI.h"
#include "Msg.h"
#include "TrapdoorMod.h"

namespace tr {

    namespace {

        CircuitSceneGraph &getCircuitSceneGraph(CircuitSystem *system) {
            // !CircuitSystem::updateDependencies
            return dAccess<CircuitSceneGraph, 8>(system);
        }

        std::string getDbgString(Actor *actor) {
            std::vector<std::string> dbgs;
            actor->getDebugText(dbgs);
            std::string res;
            for (auto &s : dbgs) res += s;
            return res;
        }
    }  // namespace

    bool displayEntityInfo(Player *player, Actor *a) {
        if (!player) return true;
        if (!a) {
            player->sendText("No actor");
            return true;
        }
        TextBuilder builder;
        builder
            .sText(TextBuilder::AQUA, "Base: \n")

            .text(" - type / UID: ")
            .sTextF(TextBuilder::GREEN, "%s    %llx\n", a->getTypeName().c_str(),
                    a->getUniqueID().get())

            .text(" - Pos/DeltaPos   AABB: ")
            .sTextF(TextBuilder::GREEN, "%s / %s|%s", fromVec3(a->getPos()).toString().c_str(),
                    fromVec3(a->getPosDelta()).toString().c_str(),
                    fromAABB(a->getAABB()).ToString().c_str())
            .text("\n")
            .text(" - Surface: ")
            .sTextF(TextBuilder::GREEN, "%d\n", a->isSurfaceMob());

        builder.sText(TextBuilder::AQUA, "Dbg:\n").textF("%s\n", getDbgString(a).c_str());
        builder.sText(tr::TextBuilder::AQUA, "NBT:\n")
            .textF("  %s", a->getNbt()->toPrettySNBT(true).c_str());
        player->sendText(builder.get());
        return false;
    }

    bool displayBlockInfo(Player *p, const BlockPos &pos) {
        if (!p) return true;
        auto &b = p->getRegion().getBlock(pos);
        if (b.getTypeName() == "minecraft:air") {
            p->sendText("get blockName failure");
        }

        tr::TextBuilder builder;
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

        // builder.sText(tr::TextBuilder::AQUA, "NBT:\n")
        //     .textF("  %s", b.get->toSNBT().c_str());
        if (b.hasBlockEntity()) {
            auto be = p->getRegion().getBlockEntity(pos);
            if (be) {
                builder.sText(tr::TextBuilder::AQUA, "BlockEntity:\n")
                    .textF("  %s", be->getNbt()->toPrettySNBT(true).c_str());
            }
        }
        p->sendText(builder.get());
        return false;
    }

    bool displayEnvInfo() { return true; }

    bool displayRedstoneCompInfo(Player *p, const BlockPos &pos) {
        tr::logger().debug("test");
        auto &cs = p->getDimension().getCircuitSystem();
        auto &graph = getCircuitSceneGraph(&cs);
        auto comp = graph.getBaseComponent(pos);
        if (!comp) {
            p->sendText("Not an redstone component");
            return false;
        }
        p->sendText("Strength =>  " + std::to_string(comp->getStrength()));

        auto &block = p->getRegion().getBlock(pos);
        auto &mobs = p->getRegion().getBiome(pos).getMobs();
        // blockName.getMobToSpawn("");
        return false;
    }
}  // namespace tr
