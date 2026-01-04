#include <stdio.h>
#include <string.h>

/*Дефиниране на максимални стойности.Те ограничават размера на данните и предотвратяват buffer overflow.*/
#define MAX_SURVEYS 10
#define MAX_QUESTIONS 10
#define MAX_FEEDBACK 100
#define MAX_TEXT 200

/*Файлове за съхранение на данните.Осигуряват постоянство между стартиранията на програмата.*/
#define SURVEY_FILE "surveys.txt"
#define FEEDBACK_FILE "feedback.txt"



/*Структура за един въпрос.Съдържа само текст, но позволява лесно разширяване.*/
typedef struct {
    char text[MAX_TEXT];
} Question;

/*Структура за анкета.Съдържа заглавие, масив от въпроси и брой въпроси.*/
typedef struct {
    char title[MAX_TEXT];
    Question questions[MAX_QUESTIONS];
    int questionCount;
} Survey;

/*Структура за отговор от студент.Свързва анкета по заглавие, име на студент и оценки.*/
typedef struct {
    char surveyTitle[MAX_TEXT];
    char studentName[MAX_TEXT];
    int ratings[MAX_QUESTIONS];
} Feedback;

/*Глобални масиви за анкетите и обратната връзка.Подходящо решение за учебен проект без динамична памет.*/
Survey surveys[MAX_SURVEYS];
Feedback feedbacks[MAX_FEEDBACK];
int surveyCount = 0;
int feedbackCount = 0;



void roleMenu();
void educatorMenu();
void studentMenu();

void createSurvey();
void listSurveys();
void fillSurvey();
void viewResults();

void saveSurveys();
void loadSurveys();
void saveFeedback(Feedback *f, int qCount);
void loadFeedback();

void clearInput();



/* Главна функция.Първо зарежда данните от файловете,след което стартира менюто.*/
int main(void) {
    loadSurveys();
    loadFeedback();
    roleMenu();
    return 0;
}



/* Изчиства входния буфер. Необходимо при смесване на scanf и fgets.*/
void clearInput() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}


/* Основно меню за избор на роля.Реализира разделение на достъпа – Educator и Student.*/
void roleMenu() {
    int choice;
    do {
        printf("\n SURVEY SYSTEM \n");
        printf("1. Educator\n");
        printf("2. Student\n");
        printf("0. Exit\n");
        printf("Choose role: ");

        if (scanf("%d", &choice) != 1) {
            clearInput();
            continue;
        }
        clearInput();

        if (choice == 1) educatorMenu();
        else if (choice == 2) studentMenu();
        else if (choice == 0) printf("Goodbye!\n");
        else printf("Invalid choice!\n");

    } while (choice != 0);
}



/*Меню за преподавател.Позволява създаване на анкети и преглед на резултатите.*/
void educatorMenu() {
    int choice;
    do {
        printf("\n--- EDUCATOR MENU ---\n");
        printf("1. Create survey\n");
        printf("2. List surveys\n");
        printf("3. View results\n");
        printf("0. Back\n");
        printf("Choose option: ");

        if (scanf("%d", &choice) != 1) {
            clearInput();
            continue;
        }
        clearInput();

        if (choice == 1) createSurvey();
        else if (choice == 2) listSurveys();
        else if (choice == 3) viewResults();
        else if (choice == 0) return;
        else printf("Invalid option!\n");

    } while (1);
}


/* Меню за студент. Студентът може да разглежда и попълва анкети.*/
void studentMenu() {
    int choice;
    do {
        printf("\n--- STUDENT MENU ---\n");
        printf("1. List surveys\n");
        printf("2. Fill survey\n");
        printf("0. Back\n");
        printf("Choose option: ");

        if (scanf("%d", &choice) != 1) {
            clearInput();
            continue;
        }
        clearInput();

        if (choice == 1) listSurveys();
        else if (choice == 2) fillSurvey();
        else if (choice == 0) return;
        else printf("Invalid option!\n");

    } while (1);
}



/* Създаване на нова анкета.Проверява дали не е достигнат максималният брой анкети.*/
void createSurvey() {
    if (surveyCount >= MAX_SURVEYS) {
        printf("Survey limit reached!\n");
        return;
    }

    Survey *s = &surveys[surveyCount];

    printf("Survey title: ");
    fgets(s->title, MAX_TEXT, stdin);
    s->title[strcspn(s->title, "\n")] = 0;

    printf("Number of questions: ");
    scanf("%d", &s->questionCount);
    clearInput();

    if (s->questionCount > MAX_QUESTIONS)
        s->questionCount = MAX_QUESTIONS;

    for (int i = 0; i < s->questionCount; i++) {
        printf("Question %d: ", i + 1);
        fgets(s->questions[i].text, MAX_TEXT, stdin);
        s->questions[i].text[strcspn(s->questions[i].text, "\n")] = 0;
    }

    surveyCount++;
    saveSurveys();

    printf("Survey saved successfully!\n");
}



/*Извежда всички налични анкети.*/
void listSurveys() {
    if (surveyCount == 0) {
        printf("No surveys available.\n");
        return;
    }

    for (int i = 0; i < surveyCount; i++)
        printf("%d. %s\n", i + 1, surveys[i].title);
}



/*Попълване на анкета от студент.Включва проверки за невалиден вход.*/
void fillSurvey() {
    if (surveyCount == 0) {
        printf("No surveys to fill.\n");
        return;
    }

    int choice;
    listSurveys();
    printf("Choose survey number: ");

    if (scanf("%d", &choice) != 1) {
        clearInput();
        return;
    }
    clearInput();

    if (choice < 1 || choice > surveyCount) {
        printf("Invalid choice.\n");
        return;
    }

    Survey *s = &surveys[choice - 1];
    Feedback *f = &feedbacks[feedbackCount];

    strcpy(f->surveyTitle, s->title);

    printf("Your name: ");
    fgets(f->studentName, MAX_TEXT, stdin);
    f->studentName[strcspn(f->studentName, "\n")] = 0;

    for (int i = 0; i < s->questionCount; i++) {
        int rating;
        printf("%s (1-5): ", s->questions[i].text);

        if (scanf("%d", &rating) != 1) {
            clearInput();
            rating = 3; /* неутрална стойност при грешка */
        }
        clearInput();

        if (rating < 1 || rating > 5)
            rating = 3;

        f->ratings[i] = rating;
    }

    saveFeedback(f, s->questionCount);
    feedbackCount++;

    printf("Thank you! Survey completed.\n");
}



/*Изчислява и показва средните оценки за всяка анкета.*/
void viewResults() {
    if (surveyCount == 0) {
        printf("No surveys available.\n");
        printf("Press ENTER to return...");
        getchar();
        return;
    }

    int hasResults = 0;

    for (int i = 0; i < surveyCount; i++) {
        Survey *s = &surveys[i];
        float avg[MAX_QUESTIONS] = {0};
        int count = 0;

        for (int f = 0; f < feedbackCount; f++) {
            if (strcmp(feedbacks[f].surveyTitle, s->title) == 0) {

                /* натрупване на оценките за всеки въпрос */
                for (int q = 0; q < s->questionCount; q++) {
                    avg[q] += feedbacks[f].ratings[q];
                }
                count++;
            }
        }

        if (count == 0) continue;

        hasResults = 1;

        printf("\nSurvey: %s\n", s->title);
        for (int q = 0; q < s->questionCount; q++) {
            printf("%s -> Average: %.2f\n",
                   s->questions[q].text,
                   avg[q] / count);
        }
    }

    if (!hasResults) {
        printf("\nNo results yet.\n");
    }

    printf("\n--- End of Results ---\n");
    printf("Press ENTER to return...");
    getchar();
}



/*Записва всички анкети във файл (overwrite).*/
void saveSurveys() {
    FILE *f = fopen(SURVEY_FILE, "w");
    if (!f) return;

    fprintf(f, "%d\n", surveyCount);

    for (int i = 0; i < surveyCount; i++) {
        fprintf(f, "%s\n%d\n",
                surveys[i].title,
                surveys[i].questionCount);

        for (int q = 0; q < surveys[i].questionCount; q++)
            fprintf(f, "%s\n", surveys[i].questions[q].text);
    }

    fclose(f);
}

/*Зарежда анкетите от файл.*/
void loadSurveys() {
    FILE *f = fopen(SURVEY_FILE, "r");
    if (!f) return;

    fscanf(f, "%d\n", &surveyCount);

    for (int i = 0; i < surveyCount; i++) {
        fgets(surveys[i].title, MAX_TEXT, f);
        surveys[i].title[strcspn(surveys[i].title, "\n")] = 0;

        fscanf(f, "%d\n", &surveys[i].questionCount);

        for (int q = 0; q < surveys[i].questionCount; q++) {
            fgets(surveys[i].questions[q].text, MAX_TEXT, f);
            surveys[i].questions[q].text[strcspn(
                surveys[i].questions[q].text, "\n")] = 0;
        }
    }

    fclose(f);
}



/*Записва попълнена анкета във файл (append).*/
void saveFeedback(Feedback *fb, int qCount) {
    FILE *f = fopen(FEEDBACK_FILE, "a");
    if (!f) return;

    fprintf(f, "%s\n%s\n", fb->surveyTitle, fb->studentName);

    for (int i = 0; i < qCount; i++)
        fprintf(f, "%d ", fb->ratings[i]);

    fprintf(f, "\n");
    fclose(f);
}

/*Зарежда всички отговори от файл.*/
void loadFeedback() {
    FILE *f = fopen(FEEDBACK_FILE, "r");
    if (!f) return;

    while (feedbackCount < MAX_FEEDBACK &&
           fgets(feedbacks[feedbackCount].surveyTitle, MAX_TEXT, f)) {

        feedbacks[feedbackCount].surveyTitle[strcspn(
            feedbacks[feedbackCount].surveyTitle, "\n")] = 0;

        fgets(feedbacks[feedbackCount].studentName, MAX_TEXT, f);
        feedbacks[feedbackCount].studentName[strcspn(
            feedbacks[feedbackCount].studentName, "\n")] = 0;

        for (int i = 0; i < MAX_QUESTIONS; i++)
            fscanf(f, "%d", &feedbacks[feedbackCount].ratings[i]);

        feedbackCount++;
    }

    fclose(f);
}
