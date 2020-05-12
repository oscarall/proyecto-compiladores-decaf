/*
 * file:  dpp.l
 * ------------
 * Lex inupt file to generate the scanner for the preprocessor.
 * If you decide to use lex for the preprocesor, put your rules
 * here, otherwise the file can remain empty.
 */
%{
  #include <stdio.h>
  #include "errors.h"
  #include <string.h>

  struct directive* get_directive(char *);
  int add_directive(char *, char *);
  int line = 1;

  struct directive {
      char *directive_name;
      char *value;
      struct directive *next;
  };

  char *directive_name;
  char *value;
%}

%x COMMENT DEFINE BADDIRECTIVE DIRECTIVE SPACEVALUE VALUE REPLACE

%%
"//".*  {}
"/*"  { 
        BEGIN(COMMENT); 
      }
<COMMENT>"*/" { BEGIN(INITIAL); }
<COMMENT><<EOF>>  { ReportError::UntermComment();
                    BEGIN(INITIAL);
                  }
<COMMENT>[^/*\n]+ |
<COMMENT>[*/] {}
<COMMENT>\n { ECHO;
              line++; 
            }

"#define" { 
            BEGIN(DEFINE);
          }
<DEFINE>" " {
              BEGIN(DIRECTIVE);
            }
<DIRECTIVE>[A-Z]+ {
                    directive_name = (char *) malloc(strlen(yytext)+1);
                    strcpy(directive_name, yytext);
                    BEGIN(SPACEVALUE);
                  }
<DIRECTIVE>. {
              BEGIN(BADDIRECTIVE);
            }
<BADDIRECTIVE>. {}
<BADDIRECTIVE>\n  { ReportError::InvalidDirective(line);
                    ECHO;
                    line++;
                    BEGIN(INITIAL);
                }
<SPACEVALUE>" " {
                  BEGIN(VALUE);
                }
<VALUE>.* { 
            value = (char *) malloc(strlen(yytext)+1);
            strcpy(value, yytext);
            add_directive(directive_name, value);
            BEGIN(INITIAL);
          }
"#" {
      BEGIN(REPLACE);
    }
<REPLACE>[A-Z]+ {
                  struct directive *dir = get_directive(yytext);
                  if (dir == NULL) {
                    ReportError::InvalidDirective(line);
                  } else {
                    printf("%s", dir->value);
                  }
                  BEGIN(INITIAL);
                }
<REPLACE>.  {
              BEGIN(BADDIRECTIVE);
            }
\n  { ECHO; line++; }

%%

struct directive *directive_list;

extern void *malloc();

int add_directive(char *directive_name, char *value)
{
      struct directive *dir;
      dir = get_directive(directive_name);
      if(dir != NULL) {
            strcpy(dir->value, value);
            return 1;
      }

      dir = (struct directive *) malloc(sizeof(struct directive));

      dir->next = directive_list;

      dir->directive_name = (char *) malloc(strlen(directive_name)+1);
      strcpy(dir->directive_name, directive_name);
      dir->value = (char *) malloc(strlen(value)+1);
      strcpy(dir->value, value);
      directive_list = dir;
      return 1;
}

struct directive* get_directive(char *directive_name)
{
      struct directive *dir = directive_list;

      for(; dir; dir = dir->next) {
        if(strcmp(dir->directive_name, directive_name) == 0) return dir;
      }

      return NULL;
}