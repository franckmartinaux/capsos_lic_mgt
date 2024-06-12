srcdir = ./src
bindir = ./bin
docdir = ./doc
savedir = ./save
sqldir = ./sql
scriptdir = ./script

CC = gcc
CPSAVE = cp $(SRC) $(HEAD) $(SQL) $(SCRIPT) $(savedir)/ 
CPRESTORE = cp $(wildcard $(savedir)/*) $(srcdir)/
RM = -rm -rf $(bindir) $(docdir)
CFLAGS = -Wall -g
LDFLAGS = -L/usr/lib64/mysql -lm -lmysqlclient -lcrypt
README = README.md
SRC = $(wildcard $(srcdir)/*.c)
HEAD = $(wildcard $(srcdir)/*.h)
PROGS = $(patsubst $(srcdir)/%.c, $(bindir)/%, $(SRC))
SQL = $(wildcard $(sqldir)/*.sql)
SCRIPT = $(wildcard $(scriptdir)/*.sh)
ARCHIVE_NAME = capsAuthentification
DOC_NAME = capsAuthentification

.PHONY: clean save restore all doc run give force_rebuild

all: $(PROGS)

$(bindir)/%: $(srcdir)/%.c $(HEAD)
	@echo "Création de l'exécutable $@.exe"
	@mkdir -p $(bindir) # Crée le répertoire bindir s'il n'existe pas
	@$(CC) $(CFLAGS) $^ -o $@.exe $(LDFLAGS)

clean:
	@echo "Suppression des exécutables $(PROGS) et de la doc associée"
	@$(RM)

save: $(SRC) $(HEAD) $(SQL) $(SCRIPT)
	@echo "Création d'une sauvegarde"
	@mkdir -p $(savedir)/ # Crée un répertoire savedir s'il n'existe pas
	@$(CPSAVE)

ifneq ("$(wildcard $(savedir))","")
restore: $(savedir)/*
	@echo "Restauration de la sauvegarde"
	@$(CPRESTORE)
else
restore:
	@echo "Aucune sauvegarde précédente trouvée, échec de restauration"
endif

generate_config:
	@echo "Création et configuration du fichier Doxyfile"
	@mkdir -p $(docdir) # Crée un répertoire docdir s'il n'existe pas
	@doxygen -g $(docdir)/Doxyfile > /dev/null 2>&1 #permet de ne pas afficher la sortie
	@sed -i 's/EXTRACT_ALL\s*=.*/EXTRACT_ALL = NO/' $(docdir)/Doxyfile
	@sed -i 's|INPUT\s*=.*|INPUT = . $(srcdir)|' $(docdir)/Doxyfile
	@sed -i 's/GENERATE_LATEX\s*=.*/GENERATE_LATEX = NO/' $(docdir)/Doxyfile
	@sed -i 's/OUTPUT_LANGUAGE\s*=.*/OUTPUT_LANGUAGE = French/' $(docdir)/Doxyfile
	@sed -i 's|OUTPUT_DIRECTORY\s*=.*|OUTPUT_DIRECTORY = $(docdir)|' $(docdir)/Doxyfile
	@sed -i 's/PROJECT_NAME\s*=.*/PROJECT_NAME = "$(DOC_NAME)"/' $(docdir)/Doxyfile
	@sed -i 's|USE_MDFILE_AS_MAINPAGE\s*=.*|USE_MDFILE_AS_MAINPAGE = README.md|' $(docdir)/Doxyfile

doc: generate_config
	@echo "Création de la documentation doxygen"
	@doxygen $(docdir)/Doxyfile > /dev/null 2>&1 #permet de ne pas afficher la sortie standard

give:
	@echo "Creation of the archive $(ARCHIVE_NAME).tar.gz"
	@mkdir -p $(ARCHIVE_NAME)/$(srcdir)
	@cp -r $(srcdir)/* $(ARCHIVE_NAME)/$(srcdir)
	@mkdir -p $(ARCHIVE_NAME)/$(sqldir)
	@cp -r $(sqldir)/* $(ARCHIVE_NAME)/$(sqldir)
	@mkdir -p $(ARCHIVE_NAME)/$(scriptdir)
	@cp -r $(scriptdir)/* $(ARCHIVE_NAME)/$(scriptdir)
	@cp $(README) Makefile ./httpd.conf $(ARCHIVE_NAME)
	@-tar czf $(ARCHIVE_NAME).tar.gz $(ARCHIVE_NAME)
	@rm -rf $(ARCHIVE_NAME)

firstRun:
	@echo "Starting MySQL service"
	@sudo systemctl start mysqld
	@echo "MySQL will ask for your root password"
	@echo "Adding user capsule to the database with the granted privileges needed"
	@mysql -u root -p -e "source $(sqldir)/addUser.sql" > /dev/null 2>&1
	@echo "Setting up the database tables for the capsule user"
	@mysql -u capsule -p'Capsule2024!' -e "source $(sqldir)/capsAuthentification.sql" > /dev/null 2>&1
	@echo "Executing Makefile commands"
	@make | awk 'NR>1 {if (p) print p; p=$$0}'
	@echo "Setting up the license expiry cron job"
	@./$(scriptdir)/updateLicenceExpiracy.sh