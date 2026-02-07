# Python API Backend

This project implements a RESTful backend service using FastAPI, designed to demonstrate
clean API design, validation, and extensible backend architecture.

The emphasis is on correctness, maintainability, and clear separation of concerns rather than
framework-specific features.

## Architecture Overview

The service exposes standard CRUD endpoints and uses:
- FastAPI for request routing and validation
- In-memory data storage for simplicity
- Explicit input validation to ensure data integrity

The codebase is structured to make future expansion straightforward without major refactoring.

## Features

- RESTful CRUD endpoints (POST, GET, PUT, DELETE)
- Request and response validation
- Clean separation between API logic and data handling
- Clear error handling and predictable behavior

## Design Philosophy

FastAPI is used as a lightweight framework, but the project focuses on backend fundamentals:
- API contract clarity
- Data validation
- Maintainable structure
- Ease of testing and deployment

The architecture avoids unnecessary coupling, making it adaptable to other frameworks or
deployment environments.

## Planned Improvements

- Persistent data storage (SQL or NoSQL)
- User authentication and authorization
- Automated testing
- Deployment and CI/CD integration

## Purpose

This project demonstrates how to design a backend service that is easy to reason about,
extend, and maintain—skills that transfer across languages, frameworks, and production systems.

## Running the Project

1. Create and activate a virtual environment:
   ```bash
   python -m venv .venv
   source .venv/bin/activate   # Linux/Mac
   .venv\Scripts\activate      # Windows

2. Install dependencies
    ```bash
   pip install -r requirements.txt

4. Run the server
    ```bash
    uvicorn app.main:app --reload

5. Visit the docs at: http://127.0.0.1:8000/docs
