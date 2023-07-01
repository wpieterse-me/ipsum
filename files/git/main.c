#include <stdint.h>
#include <stdio.h>

#include <git2.h>

int32_t main(int32_t argument_count, char** arguments) {
  git_libgit2_init();

  git_repository* repository = NULL;

  int error = git_repository_open(
      &repository,
      "/home/wpieterse/repositories/github.com/wpieterse/bazel-toolchains");
  if(error < 0) {
    const git_error* e = git_error_last();
    printf("Error %d/%d: %s\n", error, e->klass, e->message);
    return 1;
  }

  git_object* rev = NULL;
  error           = git_revparse_single(&rev, repository, "HEAD^{tree}");
  if(error < 0) {
    const git_error* e = git_error_last();
    printf("Error %d/%d: %s\n", error, e->klass, e->message);
    return 1;
  }

  git_tree* tree         = (git_tree*)rev;
  size_t    tree_entries = git_tree_entrycount(tree);
  for(size_t index = 0; index < tree_entries; index++) {
    const git_tree_entry* entry = git_tree_entry_byindex(tree, index);

    if(git_tree_entry_type(entry) == GIT_OBJECT_TREE) {
      printf("D - ");
    } else if(git_tree_entry_type(entry) == GIT_OBJECT_BLOB) {
      printf("F - ");
    } else {
      printf("U - ");
    }

    printf("%s\n", git_tree_entry_name(entry));
  }

  printf("DONE\n");
  return 0;
}
