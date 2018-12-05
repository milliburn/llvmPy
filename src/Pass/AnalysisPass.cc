// #include <llvmPy/AST.h>
// #include <llvmPy/Pass/AnalysisPass.h>
// #include <llvmPy/Pass/Analysis/FuncTree.h>
// #include <llvmPy/Pass/Analysis/ModuleTree.h>
// #include <llvmPy/Pass/Analysis/ScopeTree.h>
// using namespace llvmPy;
//
// AnalysisPassO
// AnalysisPass::run(AnalysisPassI const &input)
// {
//     auto moduleTree = buildModuleLexicalTree(input.moduleName, input.stmt);
//     return AnalysisPassO({ .moduleTree = std::move(moduleTree) });
// }
//
// std::shared_ptr<ModuleTree>
// AnalysisPass::buildModuleLexicalTree(
//         std::string const &name,
//         std::shared_ptr<Stmt const> const &stmt) const
// {
//     auto module = std::make_shared<LexicalTree>();
//     module->setName(name);
//
//     auto moduleTree = std::make_shared<ModuleTree>();
//     moduleTree->setName(name);
//
//     // auto lexicalTree = buildBasicLexicalTree(stmt);
//     // moduleTree->setBody(lexicalTree);
//
//     return moduleTree;
// }
//
// void
// AnalysisPass::buildBasicLexicalTree(LexicalTree &tree, Stmt const &stmt) const
// {
//     if (auto *def = stmt.cast<DefStmt>()) {
//
//         auto child = tree.createChild();
//         child.setStatement(stmt);
//         buildBasicLexicalTree(child, def->getBody());
//
//     } else if (auto *expr = stmt.cast<ExprStmt>()) {
//
//         buildBasicLexicalTree(tree, expr->getExpr());
//
//     } else {
//
//         for (auto const &stmt_ : stmt.getInnerStatements()) {
//             buildBasicLexicalTree(tree, stmt_);
//         }
//
//     }
// }
//
// void
// AnalysisPass::buildBasicLexicalTree(LexicalTree &tree, Expr const &expr) const
// {
//     if (auto *lambda = expr.cast<LambdaExpr>()) {
//
//         // TODO: XXX The ReturnStmt shouldn't take a dual shared_ptr to `expr`.
//         auto child = tree.createChild();
//         Stmt const *ret = new ReturnStmt(std::shared_ptr<Expr const>(&expr));
//         child.setStatement(*ret);
//
//     } else {
//
//         for (auto const &expr_ : expr.getInnerExpressions()) {
//             buildBasicLexicalTree(tree, expr_);
//         }
//
//     }
// }
//
// void
// AnalysisPass::initScopeSlotsFromSignature(
//         ScopeTree &st,
//         FuncTree const &ft) const
// {
//     for (auto const &argName : ft.getArgNames()) {
//         st.declareSlot(argName);
//     }
// }
//
// void
// AnalysisPass::initScopeSlotsFromBody(
//         ScopeTree &st,
//         Stmt const &stmt) const
// {
//     if (auto *assign = stmt.cast<AssignStmt>()) {
//         st.declareSlot(assign->getName());
//     } else if (auto *def = stmt.cast<DefStmt>()) {
//         st.declareSlot(def->getName());
//     } else if (auto *compound = stmt.cast<CompoundStmt>()) {
//         for (auto const &stmt_ : compound->getStatements()) {
//             initScopeSlotsFromBody(st, *stmt_);
//         }
//     } else if (auto *cond = stmt.cast<ConditionalStmt>()) {
//         initScopeSlotsFromBody(st, cond->getThenBranch());
//         initScopeSlotsFromBody(st, cond->getElseBranch());
//     } else if (auto *loop = stmt.cast<WhileStmt>()) {
//         initScopeSlotsFromBody(st, loop->getBody());
//     }
// }
//
// void
// AnalysisPass::initScopeFrames(
//         ScopeTree &st,
//         FuncTree const &ft) const
// {
//
// }
